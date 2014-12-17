#pragma once
#include <string>
#include <iostream>
#include <fstream>
#include <cassert>
#include <mpi.h>
#include "sparse_matrix.h"
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


void LoadSparseMatrix (const string &partFile, SparseMatrix &A) {
    ifstream ifs(partFile);
    int nRow, nCol, nNnz;
    ifs >> nRow >> nCol >> nNnz;
    assert(nRow == nCol);
    A.globalNumberOfRows = nRow;
    A.globalNumberOfNonzeros = nNnz;
    
}
