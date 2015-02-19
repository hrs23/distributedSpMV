#include <iostream>
#include <vector>
#include <cstdlib>
using namespace std;
int main () {
    /* //dense_upper
    int N = 10000;
    cout << N << " " << N << " " << N * N / 2 << endl;
    for (int i = 0; i < N/2; i++) {
        for (int j = 0; j < N; j++) {
            cout << i+1 << " " << j+1 << " " << 1.0 << endl;
        }
    }
    */
    // random integer
    int N = 10000;
    vector<pair<int,int> > es;
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            if (rand()%10 == 1) es.push_back(make_pair(i, j));
        }
    }
    cout << N << " " << N << " " << es.size() << endl;
    for (int i = 0; i < es.size(); i++) {
        cout << es[i].first + 1<< " " << es[i].second+1 << " " << i*i << endl;
    }

}
