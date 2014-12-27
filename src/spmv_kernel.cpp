#include <mkl.h>
#include "sparse_matrix.hpp"
#include "vector.h"
inline int SpMVLocal (const SparseMatrix & A, Vector & x, Vector & y) {
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    double * xv = x.values;
    double * yv = y.values;
    double ALPHA = 1;
    double BETA = 0;
    int nrow = A.localNumberOfRows;
    int ncol = A.localNumberOfRows;
    int * ptr = A.localPtr;
    int * idx = A.localIdx;
    double * val = A.localVal;
    MKL_INT * ptr_b = static_cast<MKL_INT*>(ptr);
    MKL_INT * ptr_e = ptr_b + 1;
    char transa = 'N';
    char *matdescra = "GLNC";
    mkl_dcsrmv(&transa, &nrow, &ncol, &ALPHA, matdescra, val, idx, ptr_b, ptr_e, xv, &BETA, yv);
    return 0;
}
inline int SpMVExternal (const SparseMatrix & A, Vector & x, Vector & y) {
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    double * xv = x.values;
    double * yv = y.values;
    double ALPHA = 1;
    double BETA = 1;
    int nrow = A.localNumberOfRows;
    int ncol = A.localNumberOfRows + A.totalToBeReceived;
    int * ptr = A.externalPtr;
    int * idx = A.externalIdx;
    double * val = A.externalVal;

    MKL_INT * ptr_b = static_cast<MKL_INT*>(ptr);
    MKL_INT * ptr_e = ptr_b + 1;
    char transa = 'N';
    char *matdescra = "GLNC";
    mkl_dcsrmv(&transa, &nrow, &ncol, &ALPHA, matdescra, val, idx, ptr_b, ptr_e, xv, &BETA, yv);
    return 0;
}
