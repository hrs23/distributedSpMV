#pragma once
#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <set>
#include <map>
#include <cassert>
#include <sstream>
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

string GetBasename (const string &path);

vector<Element> GetElementsFromFile (const string &mtxFile, int &nRow, int &nCol, int &nNnz);
void GetHypergraphPartitioning (int nPart, int nCell, int nNet, int nConst, int *weights, int *costs, int *xpins, int *pins, int *idx2part);

//------------------------------------------
// template
//------------------------------------------
template<class T> 
ostream& operator << (ostream &os, const vector<T> &v) { 
    os << "["; 
    for (typename vector<T>::const_iterator it = v.begin(); it != v.end(); it++) { 
        os << (it != v.begin() ? ", " : "") << *it; 
    } 
    os << "]"; return os; 
}
template<class T> 
ostream& operator << (ostream &os, const set<T> &s) { 
    os << "["; 
    for (typename set<T>::const_iterator it = s.begin(); it != s.end(); it++) { 
        os << (it != s.begin() ? ", " : "") << *it; 
    } 
    os << "]"; return os; 
}
template<class Key, class Val> 
ostream& operator << (ostream &os, const map<Key, Val> &m) { 
    os << "{"; 
    for (typename map<Key, Val>::const_iterator it = m.begin(); it != m.end(); it++) { 
        os << (it != m.begin() ? ", " : "") << it->first << ":" << it->second; 
    } 
    os << "}"; return os; 
}
template<class T, class S> 
ostream& operator << (ostream &os, const pair<T, S> &p) { 
    os << "(" << p.first << ", " << p.second << ")"; 
    return os; 
}


template<class T> bool amax (T &a, const T &b) { if (a < b) { a = b; return 1; } return 0; }
template<class T> bool amin (T &a, const T &b) { if (a > b) { a = b; return 1; } return 0; }
/*inline string to_string (int n) {
    return to_string(static_cast<long long>(n));
}*/
