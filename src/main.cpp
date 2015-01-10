#include <mpi.h>
#include <ctime>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <mpi.h>
#include "sparse_matrix.h"
#include "vector.h"
#include "spmv.h"
#include "util.h"
#include "mpi_util.h"
using namespace std;
int main (int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <prefix of part file (i.e. 'partition/test.mtx')>\n", argv[0]);
        exit(1);
    }

    cerr << "Now: Init" << endl;
    MPI_Init(&argc, &argv);

    //------------------------------
    // INIT
    //------------------------------
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    if (rank == 0) cout << "[HostName]" << endl;
    MPI_Barrier(MPI_COMM_WORLD);
    PrintHostName();
    MPI_Barrier(MPI_COMM_WORLD); cerr.flush(); cout.flush();
    string  partFile = string(argv[1]) + "-" + to_string(static_cast<long long>(size)) + "-" + to_string(static_cast<long long>(rank)) + ".part"; 
    if (rank == 0) cerr << "[Now: LoadSparseMatrix]" << endl;
    MPI_Barrier(MPI_COMM_WORLD); cerr.flush(); cout.flush();
    SparseMatrix A;
    Vector x, y;
    LoadInput(partFile, A, x);
    CreateZeroVector(y, A.localNumberOfRows);
    MPI_Barrier(MPI_COMM_WORLD); cerr.flush(); cout.flush();

    //------------------------------
    // SpMV
    //------------------------------
    if (rank == 0) cerr << "[Now: ComputeSPMV]" << endl;
    SpMV(A, x, y);
    MPI_Barrier(MPI_COMM_WORLD); cerr.flush(); cout.flush();

    //------------------------------
    // DELETE
    //------------------------------
    if (rank == 0) cerr << "[Now: Delete A, x]" << endl;
    DeleteSparseMatrix(A);
    DeleteVector(x);
    MPI_Barrier(MPI_COMM_WORLD); cerr.flush(); cout.flush();

    //------------------------------
    // Verify
    //------------------------------
    //if (rank == 0) cerr << "Now: Verify" << endl;
    MPI_Barrier(MPI_COMM_WORLD); cerr.flush(); cout.flush();

    //------------------------------
    // Print 
    //------------------------------
    if (rank == 0) cerr << "[Now: Print result]" << endl;
    PrintResult(A, y);
    MPI_Barrier(MPI_COMM_WORLD); cerr.flush(); cout.flush();

    //------------------------------
    // DELETE
    //------------------------------
    if (rank == 0) cerr << "[Now: Delete y]" << endl;
    DeleteVector(y);
    MPI_Barrier(MPI_COMM_WORLD); cerr.flush(); cout.flush();


    

    //------------------------------
    // REPORT
    //------------------------------
    if (rank == 0) cerr << "[Now: Finalize]" << endl;
    MPI_Finalize();
    if (rank == 0) cerr << "Complete!!" << endl;
    return 0;
}
