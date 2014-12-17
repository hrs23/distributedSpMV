#pragma once
#include <string>
using namespace std;

struct Element {
    int row, col;
    double val;
    Element () {}
    Element (int r, int c, double v) : row(r), col(c), val(v) {}
};
struct RowComparator {
    bool operator () (const Element &e1, const Element &e2) const {
        if (e1.row == e2.row) return e1.col < e2.col;
        return e1.row < e2.row;
    }
};
struct ColComparator {
    bool operator () (const Element &e1, const Element &e2) const {
        if (e1.col == e2.col) return e1.row < e2.row;
        return e1.col < e2.col;
    }
};

string GetBasename (const string &path) {
    size_t p = path.rfind("/");
    return path.substr(p+1);
}


