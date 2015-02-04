#include <mpi.h>
#include <string>
#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <algorithm>
#include <cassert>
#include <cstring>
#include <cstdio>
#include <cmath>
using namespace std;



void PrintHostName () {
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    char hostname[256];
    char *buf = new char[size * 256];
    int namelen;
    MPI_Get_processor_name(hostname, &namelen);
    MPI_Gather(hostname, 256, MPI_CHAR, buf, 256, MPI_CHAR, 0, MPI_COMM_WORLD);
    if (rank == 0) {
        for (int i = 0; i < size; i++) {
            fprintf(stdout, "%d/%d %s\n", i, size, buf + 256*i);
        }
        fflush(stdout);
    }
    delete [] buf;
}

int main (int argc, char *argv[]) {
    MPI_Init(&argc, &argv);
    PrintHostName();
    MPI_Finalize();   
}




