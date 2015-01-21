#include <iostream>
#include <cstring>
#include <cstdlib>
using namespace std;
int main () {
    int n = 10;
    int m[n][n];
    memset(m, 0, sizeof(m));
    int nnz = 5;
    srand(time(NULL));
    for (int i = 0; i < nnz; i++) {
        m[rand()%n][rand()%n] = rand()% n;
    }
    int rnnz = 0;
    for (int i = 0; i < n; i++) for (int j = 0; j < n; j++) if (m[i][j]) rnnz++;
    cout << n << " " << n << " " << rnnz << endl;
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (m[i][j] != 0) {
                cout << i + 1 << " " << j + 1 << " " << m[i][j] << endl;
            }
        }
    }
}
