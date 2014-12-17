#pragma once
#include <string>
#include <mpi.h>
using namespace std;

void PrintHostName () {
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    char hostname[256];
    int namelen;
    MPI_Get_processor_name(hostname, &namelen);
    fprintf(stderr, "%d/%d %s\n", rank, size, hostname);
    fflush(stderr);
}
