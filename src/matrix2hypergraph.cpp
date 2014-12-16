#include <fstream>
#include <vector>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <cassert>
#include "type.hpp"
using namespace std;

template<class T>
struct ElementStruct {
    T row, col;
    double value;
    ElementStruct () {}
    ElementStruct (T r, T c, double v) : row(r), col(c), value(v) {}
    bool operator < (const ElementStruct &e) const { 
        if (row == e.row) return col < e.col;
        return row < e.row;
    }
};
typedef ElementStruct<global_int_t> GlobalElement;

int main(int argc, char *argv[])
{
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <input matrix file> <output hypergraph file>\n", argv[0]);
        exit(1);
    }
    const char* const matrixFile = argv[1];
    const char* const hypergraphFile = argv[2];

    //--------------------------------------------------------------------------------
    // Read matrix
    //--------------------------------------------------------------------------------
    ifstream ifs(matrixFile);
    if (!ifs.is_open()) {
        cerr << "File not Found" << endl;
        exit(1);
    }
    global_int_t nRow, nCol, nNnz;
    string line;
    do {
        getline(ifs, line);
    } while (line[0] == '%');
    stringstream ss(line);
    ss >> nRow >> nCol >> nNnz;
    assert(nRow == nCol);
    vector<GlobalElement> elements(nNnz);
    for (int i = 0; i < nNnz; i++) {
        global_int_t row, col;
        double value;
        ifs >> row >> col >> value;
        row--; col--; 
        elements[i] = GlobalElement(row, col, value);
    }
    sort(elements.begin(), elements.end());

    //--------------------------------------------------------------------------------
    // Write hypergraph
    //--------------------------------------------------------------------------------

    vector< vector<global_int_t> > nets(nCol);
    vector<global_int_t> weight(nRow);
    for (size_t i = 0; i < elements.size(); i++) {
        nets[elements[i].col].push_back(elements[i].row);
        weight[elements[i].row]++;
    }
    int nCell = nRow;
    int nPin = nNnz;
    int nNet = 0;
    for (int i = 0; i < nCol; i++) {
        if (nets[i].size()) {
            nNet++;
        }
    }

    ofstream ofs(hypergraphFile);
    ofs << 1    << " "  // base
        << nCell << " "  // |V|
        << nNet << " "  // |N|
        << nPin << " "  // |pins|
        << 1    << " "  // each cell has weight
        << endl;

    for (size_t i = 0; i < nets.size(); i++) {
        if (nets[i].size()) {
            for (size_t j = 0; j < nets[i].size(); j++) {
                if (j) ofs << " ";
                ofs << nets[i][j] + 1;
            }
            ofs << endl;
        }
    }
    for (int i = 0; i < nCell; i++) {
        if (i) ofs << " ";
        ofs << weight[i];
    }
    ofs << endl;
    return 0;
}
