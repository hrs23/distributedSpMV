//#include <mpi.h>
#include <ctime>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <mpi.h>
#include "mpi_util.hpp"
using namespace std;
int main (int argc, char *argv[]) {
 //   MPI_Init(&argc, &argv);
    ofstream fout;
    if (argc < 3) {
        printf("Usage: %s <submatrix> <log directory> \n", argv[0]);
        exit(1);
    }
    //==============================
    // INIT
    //==============================

    /*
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    cerr << "[HostName]" << endl;
    PrintHostName();
    string  submatrixFile = argv[1];
    if (rank == 0) cerr << "Now: LoadSparseMatrix" << endl;
    SparseMatrix A;
    LoadSparseMatrix(matrixFile, A);
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
