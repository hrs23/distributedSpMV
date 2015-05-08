#include <iostream>
#include <vector>
#include <cstdlib>
using namespace std;
typedef pair<int,int> P;
typedef vector<P> M;
M band_matrix (int N) {
    M m;
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            if (abs(i-j) < 3) m.push_back(P(i, j));
        }
    }
    return m;
}

void print_matrix (int N, M m) {
    cout << N << " " << N << " " << m.size() << endl;
    for (int i = 0; i < m.size(); i++) {
        cout << m[i].first+1 << " " << m[i].second+1 << " 1.0" << endl;
    }
}

int main () {
    int N = 100;
    print_matrix(N, band_matrix(N));
}

