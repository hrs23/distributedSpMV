#include <iostream>
#include <cassert>
#include <vector>
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <cmath>
using namespace std;
typedef pair<int,int> P;
typedef vector<P> M;
M band_matrix (int N, int nPart) {
    vector<int> assign(nPart + 1);
    {
        int remainder = N % nPart;
        int p = 0;
        for (int i = 0; i <= nPart; i++) {
            assign[i] = p;
            if (i != nPart) p += (N / nPart) + (i < remainder);
        }
        assert(p == N);
    }
    vector<int> idx2part(N);
    {
        int p = 0;
        for (int i = 0; i < nPart; i++) {
            for (int j = assign[i]; j < assign[i+1]; j++) {
                idx2part[j] = i;
            }
        }
    }
    M m;
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            if (idx2part[i] == idx2part[j]) m.push_back(P(i, j));
        }
    }
    return m;
}

M dense_matrix (int N) {
    M m;
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            m.push_back(P(i, j));
        }
    }
    return m;
}

M unbalance_matrix (int N) {
    M m;
    for (int i = 0; i < sqrt(N); i++) {
        for (int j = 0; j < N; j++) {
            m.push_back(P(i, j));
        }
    }
    return m;
}

M random_unbalance_matrix (int N) {
    M m;
    for (int i = 0; i < sqrt(N); i++) {
        int r = rand() % N;
        for (int j = 0; j < N; j++) {
            m.push_back(P(r, j));
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

int main (int argc, char* argv[]) {
    int N = 10000;
    if (argc <= 1) {
        printf("Usage: %s <type> [size]\n", argv[0]);
        exit(1);
    }
    if (argc == 3) N = atoi(argv[2]);
    if (strcmp(argv[1], "band") == 0) print_matrix(N, band_matrix(N, 64));
    else if (strcmp(argv[1], "dense") == 0) print_matrix(N, dense_matrix(N));
    else if (strcmp(argv[1], "unbalance") == 0) print_matrix(N, unbalance_matrix(N));
    else if (strcmp(argv[1], "random_unbalance") == 0) print_matrix(N, random_unbalance_matrix(N));
    else {
        printf("%s: %s: type not found\n", argv[0], argv[1]);
    }
}

