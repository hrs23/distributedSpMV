//#include <mpi.h>
#include <ctime>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <mpi.h>
#include "sparse_matrix.h"
#include "vector.h"
#include "spmv.h"
#include "util.hpp"
#include "mpi_util.hpp"
using namespace std;
int main (int argc, char *argv[]) {
    cerr << "Now: Init" << endl;
    MPI_Init(&argc, &argv);
    if (argc != 2) {
        printf("Usage: %s <prefix of part file (i.e. 'partition/test.mtx')>\n", argv[0]);
        exit(1);
    }

    //==============================
    // INIT
    //==============================
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    if (rank == 0) cout << "[HostName]" << endl;
    MPI_Barrier(MPI_COMM_WORLD);
    PrintHostName();
    string  partFile = string(argv[1]) + "-" + to_string(static_cast<long long>(size)) + "-" + to_string(static_cast<long long>(rank)) + ".part"; 
    if (rank == 0) cerr << "Now: LoadSparseMatrix" << endl;
    SparseMatrix A;
    LoadSparseMatrix(partFile, A);
    /*
    Vector x, y;

    if (rank == 0) cerr << "Now: OptimizeProblem" << endl;
    OptimizeProblem(A, x, y);

    //==============================
    // SpMV
    //==============================
    if (rank == 0) cerr << "Now: ComputeSPMV" << endl;
    ComputeSPMV(A, x, y);

    //==============================
    // DELETE
    //==============================
    if (rank == 0) cerr << "Now: Delete A, x" << endl;
    DeleteSparseMatrix(A);
    DeleteVector(x);


    //==============================
    // DELETE
    //==============================
    if (rank == 0) cerr << "Now: Delete y" << endl;
    DeleteVector(y);

    //============================== 
    // REPORT
    //==============================
    if (rank == 0) cerr << "Now: Finalize" << endl;
    MPI_Finalize();
    if (rank == 0) cerr << "Complete!!" << endl;
    */
    return 0;
}
