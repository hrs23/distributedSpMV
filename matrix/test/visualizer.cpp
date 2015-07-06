#include <iostream>
#include <sstream>
#include <vector>
#include <cstdlib>
#include <cassert>
#include <cmath>
using namespace std;
const int MAX_SIZE = 32;
int main () {
    string line;
    do {
        getline(cin, line);
    } while (line[0] == '%');
    stringstream ss(line);
    int N, M, L;
    ss >> N >> M >> L;
    assert(N == M);
    double scale = 1;
    if (N > MAX_SIZE) {
        scale = static_cast<double>(N) / MAX_SIZE;
        N = M = MAX_SIZE;
    }

    vector<vector<int>> exist(N, vector<int>(M, false));
    for (int i = 0; i < L; i++) {
        int r, c;
        double v;
        cin >> r >> c >> v;
        r--; c--;
        int rr = round(r/scale);
        int cc = round(c/scale);
        if (rr >= N) rr = N-1;
        if (cc >= N) cc = N-1;
        exist[rr][cc]++;
    }
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            if (exist[i][j]) cout << "＊";
            else cout << "ー";
        }
        cout << endl;
    }
    return 0;
}

