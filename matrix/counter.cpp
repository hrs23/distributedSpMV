#include <iostream>
#include <algorithm>
#include <vector>
#include "util.h"
using namespace std;
int main (int argc, char *argv[]) {
    int nRow, nCol, nNnz;
    string file = argv[1];
    vector<Element> es = GetElementsFromFile(file, nRow, nCol, nNnz);
    vector<pair<int,int> > cnt(nRow);
    for (int i = 0; i < nRow; i++) cnt[i].second = i;
    pair<double, int> maxValue(0, 0);
    pair<double, int> minValue(1e10, 0);
    for (int i = 0; i < es.size(); i++) {
        cnt[es[i].row].first++;
        maxValue = max(maxValue, make_pair(es[i].val, es[i].row));
        minValue = min(minValue, make_pair(es[i].val, es[i].row));
    }
    sort(cnt.rbegin(), cnt.rend());
    for (int i = 0; i < 10; i++) {
        cout << cnt[i].second << " : " << cnt[i].first << endl;
    }
    cout << "max:" << maxValue.first << " idx:" << maxValue.second << endl;
    cout << "min:" << minValue.first << " idx:" << minValue.second << endl;
}
