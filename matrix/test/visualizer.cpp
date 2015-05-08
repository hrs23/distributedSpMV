#include <iostream>
#include <vector>
#include <cstdlib>
using namespace std;

int main () {
    int N, M, L;
    cin >> N >> M >> L;
    vector<vector<bool>> exist(N, vector<bool>(M, false));
    for (int i = 0; i < L; i++) {
        int r, c;
        double v;
        cin >> r >> c >> v;
        r--; c--;
        exist[r][c] = true;
    }
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < M; j++) {
            if (exist[i][j]) cout << "*";
            else cout << "-";
        }
        cout << endl;
    }
    return 0;
}

