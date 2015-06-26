#include <iostream>
#include <omp.h>
using namespace std;
int main() {
    omp_set_num_threads(8);
    cout << omp_get_max_threads() << endl;
#pragma omp parallel num_threads(8)
    {
#pragma omp master
    cout << omp_get_max_threads() << endl;
    }
}
