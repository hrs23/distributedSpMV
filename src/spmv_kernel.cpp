#include <iostream>
#include <mkl.h>
#include <mpi.h>
#include "spmv_kernel.h"
#include "sparse_matrix.h"
#include "vector.h"
using namespace std;
int SpMVInternal (const SparseMatrix & A, Vector & x, Vector & y) {
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    double *xv = x.values;
    double *yv = y.values;
    double ALPHA = 1;
    double BETA = 0;
    int nrow = A.localNumberOfRows;
    int ncol = A.localNumberOfRows;
    int *ptr = A.internalPtr;
    int *idx = A.internalIdx;
    double *val = A.internalVal;
    MKL_INT *ptr_b = static_cast<MKL_INT*>(ptr);
    MKL_INT *ptr_e = ptr_b + 1;
    char transa = 'N';
    char *matdescra = "GLNC";
    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Barrier(MPI_COMM_WORLD);
    mkl_dcsrmv(&transa, &nrow, &ncol, &ALPHA, matdescra, val, idx, ptr_b, ptr_e, xv, &BETA, yv);
    /*
    if (rank == 2) {

        cerr << "x y" << endl;
        for (int i = 0; i < A.localNumberOfRows; i++) {
            cerr << xv[i] << " " << yv[i] << endl;
        }
        cerr << "Mat" << endl;
        for (int i = 0; i < nrow; i++) {
            cerr << i << " " << ptr[i] << " to " << ptr[i+1] << endl;
            for (int j = ptr[i]; j < ptr[i+1]; j++) {
                cerr << i << " " << idx[j] << " " << val[j] << endl;
            }
        }
    }*/
    return 0;
}
int SpMVExternal (const SparseMatrix & A, Vector & x, Vector & y) {
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    double *xv = x.values;
    double *yv = y.values;
    double ALPHA = 1;
    double BETA = 1;
    int nrow = A.localNumberOfRows;
    int ncol = A.localNumberOfRows + A.totalNumberOfRecv;
    int *ptr = A.externalPtr;
    int *idx = A.externalIdx;
    double *val = A.externalVal;

    MKL_INT *ptr_b = static_cast<MKL_INT*>(ptr);
    MKL_INT *ptr_e = ptr_b + 1;
    char transa = 'N';
    char *matdescra = "GLNC";
    mkl_dcsrmv(&transa, &nrow, &ncol, &ALPHA, matdescra, val, idx, ptr_b, ptr_e, xv, &BETA, yv);
    /*
    if (rank == 2) {
        cerr << "x y" << endl;
        for (int i = 0; i < A.localNumberOfRows; i++) {
            cerr << xv[i] << " " << yv[i] << endl;
        }
        cerr << "Mat" << endl;
        for (int i = 0; i < nrow; i++) {
            cerr << i << " " << ptr[i] << " to " << ptr[i+1] << endl;
            for (int j = ptr[i]; j < ptr[i+1]; j++) {
                cerr << i << " " << idx[j] << " " << val[j] << endl;
            }
        }
    }
    */
    return 0;
}
