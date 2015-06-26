#include <mpi.h>
#include <iostream>
#include <cstdio>
#include "spmv_kernel.h"
#include "sparse_matrix.h"
#include "vector.h"
#if defined(MIC) || defined(CPU)
#include <mkl.h>
#endif
#ifdef GPU
#include <cuda_runtime_api.h>
#include <cusparse_v2.h>
#include <helper_cuda.h>
#endif
using namespace std;

void my_dcsrmv (double coef, int nRow, int *ptr, int *idx, double *val, double *xv, double *yv) {
#pragma omp parallel for
    for (int i = 0; i < nRow; i++) {
        yv[i] += coef;
        for (int j = ptr[i]; j < ptr[i+1]; j++) {
            yv[i] += val[j] * xv[idx[j]];
        }
    }
}

int SpMVInternal (const SparseMatrix & A, Vector & x, Vector & y) {
    double *xv = x.values;
    double *yv = y.values;
    double ALPHA = 1;
    double BETA = 0;
    int nRow = A.localNumberOfRows;
    int nNnz = A.internalPtr[nRow];
    if (nNnz == 0) return 0;
#if defined(MIC) || defined(CPU)
    int *ptr = A.internalPtr;
    int *idx = A.internalIdx;
    double *val = A.internalVal;
    MKL_INT *ptr_b = static_cast<MKL_INT*>(ptr);
    MKL_INT *ptr_e = ptr_b + 1;
    char transa = 'N';
    char *matdescra = "GLNC";
#ifndef MY_CSRMV
    mkl_dcsrmv(&transa, &nRow, &nRow, &ALPHA, matdescra, val, idx, ptr_b, ptr_e, xv, &BETA, yv);
#else
    my_dcsrmv(BETA, nRow, ptr, idx, val, xv, yv);
#endif
#endif
#ifdef GPU
    int *cuda_ptr = A.cuda_internalPtr;
    int *cuda_idx = A.cuda_internalIdx;
    double *cuda_val = A.cuda_internalVal;
    double *cuda_x = A.cuda_x_values;
    double *cuda_y = A.cuda_y_values;

    checkCudaErrors(cudaMemcpy((void *)cuda_x, xv, A.localNumberOfRows * sizeof(double), cudaMemcpyHostToDevice));
    
    ::cusparseHandle_t cusparse;
    ::cusparseCreate(&cusparse);
    ::cusparseMatDescr_t matDescr;
    ::cusparseCreateMatDescr(&matDescr);
    ::cusparseSetMatType(matDescr, CUSPARSE_MATRIX_TYPE_GENERAL);
    ::cusparseSetMatIndexBase(matDescr, CUSPARSE_INDEX_BASE_ZERO);
    ::cusparseDcsrmv(cusparse, CUSPARSE_OPERATION_NON_TRANSPOSE, nRow, nRow, nNnz, &ALPHA, matDescr, cuda_val, cuda_ptr, cuda_idx, cuda_x, &BETA, cuda_y);
#endif
    return 0;
}


/*
int SpMVDenseInternal (const SparseMatrix & A, Vector & x, Vector & y) {
    double *xv = x.denseInternalValues;
    double *yv = y.values;
    double ALPHA = 1;
    double BETA = 0;
    int nRow = A.localNumberOfRows;
    int nNnz = A.internalPtr[nRow];
    if (nNnz == 0) return 0;
#if defined(MIC) || defined(CPU)
    int *ptr = A.internalPtr;
    int *idx = A.denseInternalIdx;
    double *val = A.internalVal;
    MKL_INT *ptr_b = static_cast<MKL_INT*>(ptr);
    MKL_INT *ptr_e = ptr_b + 1;
    char transa = 'N';
    char *matdescra = "GLNC";
    mkl_dcsrmv(&transa, &nRow, &nRow, &ALPHA, matdescra, val, idx, ptr_b, ptr_e, xv, &BETA, yv);
#endif
#ifdef GPU
    int *cuda_ptr = A.cuda_internalPtr;
    int *cuda_idx = A.cuda_denseInternalIdx;
    double *cuda_val = A.cuda_internalVal;
    double *cuda_x = A.cuda_x_values;
    double *cuda_y = A.cuda_y_values;

    checkCudaErrors(cudaMemcpy((void *)cuda_x, xv, A.numberOfUniqInternalCols * sizeof(double), cudaMemcpyHostToDevice));
    
    ::cusparseHandle_t cusparse;
    ::cusparseCreate(&cusparse);
    ::cusparseMatDescr_t matDescr;
    ::cusparseCreateMatDescr(&matDescr);
    ::cusparseSetMatType(matDescr, CUSPARSE_MATRIX_TYPE_GENERAL);
    ::cusparseSetMatIndexBase(matDescr, CUSPARSE_INDEX_BASE_ZERO);
    ::cusparseDcsrmv(cusparse, CUSPARSE_OPERATION_NON_TRANSPOSE, nRow, nRow, nNnz, &ALPHA, matDescr, cuda_val, cuda_ptr, cuda_idx, cuda_x, &BETA, cuda_y);
#endif
    return 0;
}
*/


int SpMVExternal (const SparseMatrix & A, Vector & x, Vector & y) {
    double *xv = x.values;
    double *yv = y.values;
    double ALPHA = 1;
    double BETA = 1;
    int nRow = A.localNumberOfRows;
    int nCol = A.localNumberOfRows + A.totalNumberOfRecv;
    int nNnz = A.externalPtr[nRow];
    if (nNnz == 0) return 0;
#if defined(MIC) || defined(CPU)
    int *ptr = A.externalPtr;
    int *idx = A.externalIdx;
    double *val = A.externalVal;
    MKL_INT *ptr_b = static_cast<MKL_INT*>(ptr);
    MKL_INT *ptr_e = ptr_b + 1;
    char transa = 'N';
    char *matdescra = "GLNC";
#ifndef MY_CSRMV
    mkl_dcsrmv(&transa, &nRow, &nCol, &ALPHA, matdescra, val, idx, ptr_b, ptr_e, xv, &BETA, yv);
#else
    my_dcsrmv(BETA, nRow, ptr, idx, val, xv, yv);
#endif

#endif

#ifdef GPU
    int * cuda_ptr = A.cuda_externalPtr;
    int * cuda_idx = A.cuda_externalIdx;
    double * cuda_val = A.cuda_externalVal;
    double * cuda_x = A.cuda_x_values;
    double * cuda_y = A.cuda_y_values;

    checkCudaErrors(cudaMemcpy((void *)(cuda_x + A.localNumberOfRows), xv + A.localNumberOfRows, A.totalNumberOfRecv * sizeof(double), cudaMemcpyHostToDevice));

    ::cusparseHandle_t cusparse;
    ::cusparseCreate(&cusparse);
    ::cusparseMatDescr_t matDescr;
    ::cusparseCreateMatDescr(&matDescr);
    ::cusparseSetMatType(matDescr, CUSPARSE_MATRIX_TYPE_GENERAL);
    ::cusparseSetMatIndexBase(matDescr, CUSPARSE_INDEX_BASE_ZERO);
    ::cusparseDcsrmv(cusparse, CUSPARSE_OPERATION_NON_TRANSPOSE, nRow, nCol, nNnz, &ALPHA, matDescr, cuda_val, cuda_ptr, cuda_idx, cuda_x, &BETA, cuda_y);

    checkCudaErrors(cudaMemcpy((void *)yv, cuda_y, nRow * sizeof(double), cudaMemcpyDeviceToHost));
#endif
    return 0;
}

