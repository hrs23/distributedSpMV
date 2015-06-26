#include <fstream>
#include <mpi.h>
#include <map>
#include <set>
#include <iostream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <cassert>



using namespace std;
double GetSynchronizedTime () {
    MPI_Barrier(MPI_COMM_WORLD);
    double t = MPI_Wtime();
    MPI_Bcast(&t, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    return t;
}
int main(int argc, char *argv[])
{
    MPI_Init(&argc, &argv);
    int N = atoi(argv[1]);

    double t = -GetSynchronizedTime();
    for (int i = 0; i < N; i++) {
        MPI_Barrier(MPI_COMM_WORLD);
    }
    t  += GetSynchronizedTime();
    printf("%.8lf\n", t / N);
    return 0;
}
