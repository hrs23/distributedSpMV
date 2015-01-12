#include <fstream>
#include <map>
#include <set>
#include <iostream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <cassert>
#include "util.h"
#include "patoh.h"
using namespace std;

void GetHypergraphPartitioning (int nPart, int nCell, int nNet, int nConst, int *weights, int *costs, int *xpins, int *pins, int *idx2part);
void CreatePartitionFiles (int nPart, const vector<Element> &elements, int nRow, int nCol, int nNnz, int *idx2part, const char *inputFile, const char *outputDir);

int main(int argc, char *argv[])
{
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <input matrix file> <number of parts> <output partition directory>\n", argv[0]);
        exit(1);
    }
    int nRow, nCol, nNnz;
    vector<Element> elements = GetElementsFromFile(argv[1], nRow, nCol, nNnz);

    int nPart = atoi(argv[2]);
    assert(nPart >= 2);
    int nPin = nNnz, nCell = nRow, nNet = nCol, nConst = 0;

    int *xnets = new int[nCell+1];
    int *nets = new int[nPin];
    int *weights = new int[nCell];
    {
        sort(elements.begin(), elements.end(), RowComparator());
        int p = 0;
        for (int i = 0; i < nNnz; i++) {
            int row = elements[i].row;
            nets[i] = elements[i].col;
            while (p <= row) { xnets[p++] = i; }
        }
        while (p <= nRow) { xnets[p++] = nNnz; }
        for (int i = 0; i < nCell; i++) {
            weights[i] = xnets[i+1] - xnets[i];
        }
    }

    int *xpins = new int[nNet+1];
    int *pins = new int[nPin];
    int *costs = new int[nNet];
    {
        sort(elements.begin(), elements.end(), ColComparator());
        int p = 0;
        for (int i = 0; i < nNnz; i++) {
            pins[i] = elements[i].row;
            int col = elements[i].col;
            while (p <= col) { xpins[p++]  = i; }
        }
        while (p <= nCol) { xpins[p++] = nNnz; }
        for (int i = 0; i < nNet; i++) {
            costs[i] = xpins[i+1] - xpins[i];
        }
    }

    int *idx2part = new int[nCell];
    GetHypergraphPartitioning(nPart, nCell, nNet, nConst, weights, costs, xpins, pins, idx2part);
    CreatePartitionFiles(nPart, elements, nRow, nCol, nNnz, idx2part, argv[1], argv[3]);

//    PaToH_Free();
    return 0;
}

void GetHypergraphPartitioning (int nPart, int nCell, int nNet, int nConst, int *weights, int *costs, int *xpins, int *pins, int *idx2part) {

    PaToH_Parameters params;
    PaToH_Initialize_Parameters(&params, PATOH_CONPART, PATOH_SUGPARAM_DEFAULT);
    params._k = nPart;
    PaToH_Alloc(&params, nCell, nNet, nConst, weights, costs, xpins, pins);
    
    int *partweights = new int[params._k * nConst];
    int cutsize;
    PaToH_Part(&params, nCell, nNet, nConst, 0, weights, costs, xpins, pins, NULL, idx2part, partweights, &cutsize);
}


void CreatePartitionFiles (int nPart, const vector<Element> &elements, int nRow, int nCol, int nNnz, int *idx2part, const char *inputFile, const char *outputDir) {
    int nCell = nRow;
    int nNet = nCol;
    int nPin = nNnz;
    vector< vector<int> > part2idx(nPart);
    for (int i = 0; i < nCell; i++) {
        part2idx[idx2part[i]].push_back(i);
    }
#pragma omp parallel for
    for (int p = 0; p < nPart; p++) {
        string dir = outputDir;
        string file = GetBasename(inputFile) + "-"
            + to_string(static_cast<long long>(nPart)) + "-"
            + to_string(static_cast<long long>(p)) + ".part";
        //cout << dir + "/" + file << endl;
        printf("%s/%s\n", dir.c_str(), file.c_str());
        ofstream ofs(dir + "/" + file);
        ofs << "#Matrix" << endl;
        ofs << nRow << " " << nCol << " " << nNnz << " " << nPart << " " << GetBasename(inputFile) << endl;
        //----------------------------------------------------------------------
        // 保持する行番号
        //----------------------------------------------------------------------
        ofs << "#Partitioning" << endl;
        for (int i = 0; i < nCell; i++) {
            if (i) ofs << " ";
            ofs << idx2part[i];
        }
        ofs << endl;

        //----------------------------------------------------------------------
        // 前計算 
        //----------------------------------------------------------------------

        vector< set<int> > sendElements(nCell); // global index of column
        vector< set<int> > recvElements(nCell); // global index of column
        for (int i = 0; i < elements.size(); i++) {
            int row = elements[i].row;
            int col = elements[i].col;
            int src = idx2part[col], dst = idx2part[row];
            if (src == p && dst != p) {
                sendElements[dst].insert(col);
            }
            if (src != p && dst == p) {
                recvElements[src].insert(col);
            }
        }
        vector<int> internalCol = part2idx[p];
        vector<int> externalCol;
        {
            for (int i = 0; i < nCell; i++) {
                for (auto it = recvElements[i].begin(); it != recvElements[i].end(); it++) {
                    externalCol.push_back(*it);
                }
            }
        }
        set<int> allCol;
        for_each(internalCol.begin(), internalCol.end(), [&](int c){ allCol.insert(c); });
        for_each(externalCol.begin(), externalCol.end(), [&](int c){ allCol.insert(c); });

        // allCol == internalCol + externalCol
        map<int, int> global2local;
        const int externalOffset = internalCol.size();
        sort(internalCol.begin(), internalCol.end());
        sort(externalCol.begin(), externalCol.end());
        for (auto it = allCol.begin(); it != allCol.end(); it++) {
            if (binary_search(internalCol.begin(), internalCol.end(), *it)) {
                int pos = lower_bound(internalCol.begin(), internalCol.end(), *it) - internalCol.begin();
                global2local[*it] = pos;
            } else {
                int pos = lower_bound(externalCol.begin(), externalCol.end(), *it) - externalCol.begin();
                global2local[*it] = pos + externalOffset;
            }
        }

        int nSendNeighbors = 0, nRecvNeighbors = 0;
        int nSendElements = 0, nRecvElements = 0;
        for (int i = 0; i < nCell; i++) {
            if (sendElements[i].size()) nSendNeighbors++;
            if (recvElements[i].size()) nRecvNeighbors++;
            nSendElements += sendElements[i].size();
            nRecvElements += recvElements[i].size();
        }
        vector<int> local2global(allCol.size());
        for (auto it = global2local.begin(); it != global2local.end(); it++) {
            local2global[it->second] = it->first;
        }
        //----------------------------------------------------------------------
        // 保持する部分行列
        //----------------------------------------------------------------------
        // row col val
        ofs << "#LocalToGlobalTable" << endl;
        ofs << allCol.size() << endl;
        for (int i = 0; i < allCol.size(); i++) {
            if (i) ofs << " ";
            ofs << local2global[i];
        }
        ofs << endl;

        ofs << "#SubMatrix" << endl;
        sort(elements.begin(), elements.end(), RowComparator());
        int localNumberOfRows = count(idx2part, idx2part+nCell, p);
        int numInternalNnz = count_if(elements.begin(), elements.end(), 
                [&](const Element &e) { return idx2part[e.row] == p && idx2part[e.col] == p; });
        int numExternalNnz = count_if(elements.begin(), elements.end(), 
                [&](const Element &e) { return idx2part[e.row] == p && idx2part[e.col] != p; });

        ofs << localNumberOfRows << " " << numInternalNnz << " " << numExternalNnz << endl;

        for_each(elements.begin(), elements.end(), 
                [&](const Element &e){ if (idx2part[e.row] == p && idx2part[e.col] == p) 
                ofs << e.row << " " << e.col << " " << e.val << endl; 
                });
        for_each(elements.begin(), elements.end(), 
                [&](const Element &e){ if (idx2part[e.row] == p && idx2part[e.col] != p) 
                ofs << e.row << " " << e.col << " " << e.val << endl; 
                });

        //----------------------------------------------------------------------
        // 通信 
        //----------------------------------------------------------------------
        ofs << "#Communication" << endl;

        ofs << "#Send" << endl;
        ofs << nSendNeighbors << " " << nSendElements << endl;
        for (int i = 0; i < sendElements.size(); i++) {
            if (sendElements[i].size()) {
                ofs << i << " " << sendElements[i].size();
                for (auto it = sendElements[i].begin(); it != sendElements[i].end(); it++) {
                    ofs << " " << global2local[*it];
                }
                ofs << endl;
            }
        }
        ofs << "#Recv" << endl;
        ofs << nRecvNeighbors << " " << nRecvElements << endl;
        for (int i = 0; i < recvElements.size(); i++) {
            if (recvElements[i].size()) {
                ofs << i << " " << recvElements[i].size();
                for (auto it = recvElements[i].begin(); it != recvElements[i].end(); it++) {
                    ofs << " " << global2local[*it];
                }
                ofs << endl;
            }
        }
        ofs.close();
    }
}
