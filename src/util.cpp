#include <algorithm>
#include "util.h"
using namespace std;
string GetBasename (const string &path) {
    size_t p = path.rfind("/");
    return path.substr(p+1);
}


vector<Element> GetElementsFromFile (const string &mtxFile, int &nRow, int &nCol, int &nNnz) {
    ifstream ifs(mtxFile);
    if (ifs.fail()) {
        cerr << "File not Found" << endl;
        exit(1);
    }
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
    sort(elements.begin(), elements.end(), RowComparator());
    return elements;
}



