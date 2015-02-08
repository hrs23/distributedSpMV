#include <mpi.h>
#include <iostream>
#include <cstdio>
#include <mkl.h>
#include "spmv_kernel.h"
#include "sparse_matrix.h"
#include "vector.h"
using namespace std;
int SpMVInternal (const SparseMatrix & A, Vector & x, Vector & y) {
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
    mkl_dcsrmv(&transa, &nrow, &ncol, &ALPHA, matdescra, val, idx, ptr_b, ptr_e, xv, &BETA, yv);
    return 0;
}
int SpMVExternal (const SparseMatrix & A, Vector & x, Vector & y) {
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
    return 0;
}
