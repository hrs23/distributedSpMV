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
#include "util.hpp"
#include "patoh.h"

using namespace std;
int main(int argc, char *argv[])
{
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <input matrix file> <number of parts> <output partition directory>\n", argv[0]);
        exit(1);
    }
    ifstream ifs(argv[1]);
    if (!ifs.is_open()) {
        cerr << "File not Found" << endl;
        exit(1);
    }
    int nRow, nCol, nNnz;
    string line;
    do {
        getline(ifs, line);
    } while (line[0] == '%');
    stringstream ss(line);
    ss >> nRow >> nCol >> nNnz;
    assert(nRow == nCol);
    vector<Element> elements(nNnz);
    for (int i = 0; i < nNnz; i++) {
        int row, col;
        double val;
        ifs >> row >> col >> val;
        row--; col--; 
        elements[i] = Element(row, col, val);
    }

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

    PaToH_Parameters params;
    PaToH_Initialize_Parameters(&params, PATOH_CONPART, PATOH_SUGPARAM_DEFAULT);
    params._k = nPart;
    int cutsize;
    int *idx2part = new int[nCell];
    int *partweights = new int[params._k * nConst];
    PaToH_Alloc(&params, nCell, nNet, nConst, weights, costs, xpins, pins);
    PaToH_Part(&params, nCell, nNet, nConst, 0, weights, costs, xpins, pins, NULL, idx2part, partweights, &cutsize);

    vector< vector<int> > part2idx(nPart);
    for (int i = 0; i < nCell; i++) {
        part2idx[idx2part[i]].push_back(i);
    }
    //#pragma omp parallel for
    for (int p = 0; p < nPart; p++) {
        string dir = argv[3];
        string file = GetBasename(argv[1]) + "-"
            + to_string(static_cast<long long>(nPart)) + "-"
            + to_string(static_cast<long long>(p)) + ".part";
        cout << dir + "/" + file << endl;
        ofstream ofs(dir + "/" + file);
        ofs << nRow << " " << nCol << " " << nNnz << " " << nPart << " " << GetBasename(argv[1]) << endl;
        //----------------------------------------------------------------------
        // 保持する行番号
        //----------------------------------------------------------------------
        ofs << "#Partitioning" << endl;
        for (int i = 0; i < nCell; i++) {
            if (i) ofs << " ";
            ofs << idx2part[i];
        }
        ofs << endl;
        /*
        for (int i = 0; i < nPart; i++) {
            ofs << part2idx[i].size();
            for (int j = 0; j < part2idx[i].size(); j++) {
                ofs << " " << part2idx[i][j];
            }
            ofs << endl;
        }
        */

        //----------------------------------------------------------------------
        // インデックスの作成 
        // グローバルな行番号 -> ローカルな行番号に変換
        // 特に通信の必要ない要素を前に持ってくる（キャッシュのため）
        //----------------------------------------------------------------------

        //----------------------------------------------------------------------
        // 保持する部分行列
        //----------------------------------------------------------------------
        // row col val
        sort(elements.begin(), elements.end(), RowComparator());
        ofs << "#SubMatrix" << endl;
        int numLocalNnz;
        for (int i = 0; i < elements.size(); i++) {
            if (idx2part[elements[i].row] == p) {
                numLocalNnz++;
            }
        }
        ofs << numLocalNnz << endl;
        for (int i = 0; i < elements.size(); i++) {
            if (idx2part[elements[i].row] == p) {
                ofs << elements[i].row << " " << elements[i].col << " " << elements[i].val << endl;
            }
        }

        //----------------------------------------------------------------------
        // 通信 
        //----------------------------------------------------------------------
        ofs << "#Communication" << endl;
        vector< set<int> > sendElements(nCell);
        vector< set<int> > recvElements(nCell);
        set<int> needCol;
        for (int i = 0; i < elements.size(); i++) {
            int row = elements[i].row;
            int col = elements[i].col;
            int src = idx2part[col], dst = idx2part[row];
            if (src == p && dst != p) {
                sendElements[dst].insert(col);
                needCol.insert(col);
            }
            if (src != p && dst == p) {
                recvElements[src].insert(col);
                needCol.insert(col);
            }
        }
        vector<int> &sendCol = part2idx[p];
        vector<int> recvCol;
        {
            set<int> colSet;
            for (int i = 0; i < nCell; i++) {
                for (auto it = recvElements[i].begin(); it != recvElements[i].end(); it++) {
                    colSet.insert(*it);
                }
                recvCol = vector<int>(colSet.begin(), colSet.end());
            }
        }
        map<int, int> global2local;
        const int externalOffset = sendCol.size();
        for (auto it = needCol.begin(); it != needCol.end(); it++) {
            if (binary_search(sendCol.begin(), sendCol.end(), *it)) {
                int pos = lower_bound(sendCol.begin(), sendCol.end(), *it) - sendCol.begin();
                global2local[*it] = pos;
            } else {
                int pos = lower_bound(recvCol.begin(), recvCol.end(), *it) - recvCol.begin();
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
        /*
           for (int i = 0; i < nCell;
           if (src != p && dst == p) {
           recv[src].push_back(
           lower_bound(part2idx[p].begin(), part2idx.end(), col) - 
           part2idx[p].begin());
           }
           */

        ofs.close();
    }

    delete [] idx2part;
    delete [] partweights;
    PaToH_Free();
    return 0;
}
