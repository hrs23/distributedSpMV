#include <iostream>
using namespace std;
int main () {
    int N = 10000;
    cout << N << " " << N << " " << N * N / 2 << endl;
    for (int i = 0; i < N/2; i++) {
        for (int j = 0; j < N; j++) {
            cout << i+1 << " " << j+1 << " " << 1.0 << endl;
        }
    }
}
