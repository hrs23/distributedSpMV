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
#include "mpi_util.h"
#include "sparse_matrix.h"
#include "vector.h"
#include "util.h"
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

void LoadInput (const string &partFile, SparseMatrix &A, Vector &x) {
    ifstream ifs(partFile);
    if (ifs.fail()) {
        std::cerr << "File not found : " + partFile<< std::endl;
        exit(1);
    }
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    string comment;

    //--------------------------------------------------------------------------------
    // Matrix
    //--------------------------------------------------------------------------------
    ifs >> comment; assert(comment == "#Matrix");
    int nCol;
    string matrixName;
    int nProc;
    ifs >> A.globalNumberOfRows >> nCol >> A.globalNumberOfNonzeros >> nProc >> matrixName;
    assert(nProc == size);
    assert(A.globalNumberOfRows == nCol);

    //--------------------------------------------------------------------------------
    // Partitioning
    //--------------------------------------------------------------------------------
    ifs >> comment; assert(comment == "#Partitioning");
    A.assign = new int[A.globalNumberOfRows];
    for (int i = 0; i < A.globalNumberOfRows; i++) ifs >> A.assign[i];

    //--------------------------------------------------------------------------------
    // Local <-> global map
    //--------------------------------------------------------------------------------
    ifs >> comment; assert(comment == "#LocalToGlobalTable");
    ifs >> A.totalNumberOfUsedCols;
    A.local2global = new int[A.totalNumberOfUsedCols];
    for (int i = 0; i < A.totalNumberOfUsedCols; i++) {
        ifs >> A.local2global[i];
        A.global2local[A.local2global[i]] = i;
    }
    //--------------------------------------------------------------------------------
    // SubMatrix
    //--------------------------------------------------------------------------------
    ifs >> comment; assert(comment == "#SubMatrix");
    int numInternalNnz, numExternalNnz;
    ifs >> A.localNumberOfRows >> numInternalNnz >> numExternalNnz;

    A.internalPtr = new int[A.localNumberOfRows + 1];
    A.internalIdx = new int[numInternalNnz];
    A.internalVal = new double[numInternalNnz];
    {
        int ip = 0;
        for (int i = 0; i < numInternalNnz; i++) {
            int row, col;
            double val;
            ifs >> row >> col >> val;
            row = A.global2local[row];
            col = A.global2local[col];
            A.internalIdx[i] = col;
            A.internalVal[i] = val;
            while (ip <= row) A.internalPtr[ip++] = i;
        }
        while (ip <= A.localNumberOfRows) A.internalPtr[ip++] = numInternalNnz;
    }
    A.externalPtr = new int[A.localNumberOfRows + 1];
    A.externalIdx = new int[numExternalNnz];
    A.externalVal = new double[numExternalNnz];
    {
        int ep = 0;
        for (int i = 0; i < numExternalNnz; i++) {
            int row, col;
            double val;
            ifs >> row >> col >> val;
            row = A.global2local[row];
            col = A.global2local[col];
            A.externalIdx[i] = col;
            A.externalVal[i] = val;
            while (ep <= row) A.externalPtr[ep++] = i;
        }
        while (ep <= A.localNumberOfRows) A.externalPtr[ep++] = numExternalNnz;
    }
    A.localNumberOfNonzeros = numInternalNnz + numExternalNnz;

    //--------------------------------------------------------------------------------
    // Communication
    //--------------------------------------------------------------------------------
    ifs >> comment; assert(comment == "#Communication");

    ifs >> comment; assert(comment == "#Send");
    ifs >> A.numberOfSendNeighbors >> A.totalNumberOfSend;
    A.sendLength = new int[A.numberOfSendNeighbors];
    A.sendNeighbors = new int[A.numberOfSendNeighbors];
    A.sendBuffer = new double[A.totalNumberOfSend];
    A.localIndexOfSend = new int[A.totalNumberOfSend];
    int sendOffset = 0;
    for (int i = 0; i < A.numberOfSendNeighbors; i++) {
        ifs >> A.sendNeighbors[i] >> A.sendLength[i];
        for (int j = 0; j < A.sendLength[i]; j++) {
            ifs >> A.localIndexOfSend[sendOffset + j];
        }
        sendOffset += A.sendLength[i];
    }
    assert(sendOffset == A.totalNumberOfSend);

    ifs >> comment; assert(comment == "#Recv");
    ifs >> A.numberOfRecvNeighbors >> A.totalNumberOfRecv;
    A.recvLength = new int[A.numberOfRecvNeighbors];
    A.recvNeighbors = new int[A.numberOfRecvNeighbors];
    A.localIndexOfRecv = new int[A.totalNumberOfRecv];
    int recvOffset = 0;
    for (int i = 0; i < A.numberOfRecvNeighbors; i++) {
        ifs >> A.recvNeighbors[i] >> A.recvLength[i];
        for (int j = 0; j < A.recvLength[i]; j++) {
            ifs >> A.localIndexOfRecv[recvOffset + j];
        }
        recvOffset += A.recvLength[i];
    }
    assert(recvOffset == A.totalNumberOfRecv);
    x.values = new double[A.totalNumberOfUsedCols];
    for (int i = 0; i < A.localNumberOfRows; i++) {
        x.values[i] = A.local2global[i] + 1;
    }
    //fill(x.values, x.values + A.totalNumberOfUsedCols, 1);
}

void CreateZeroVector (Vector &v, int length) {
    v.values = new double[length];
    fill(v.values, v.values + length, 0);
}

void PrintResult (SparseMatrix &A, Vector &y) {
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Barrier(MPI_COMM_WORLD);
    for (int i = 0; i < A.globalNumberOfRows; i++) {
        MPI_Barrier(MPI_COMM_WORLD);
        if (A.assign[i] == rank) {
            cerr << i << " " << rank << " " <<  A.global2local[i] << " " << y.values[A.global2local[i]] << endl;
        }
        cerr.flush();
        MPI_Barrier(MPI_COMM_WORLD);
    }
}


bool VerifySpMV (const char *mtxFile, const SparseMatrix &A, const Vector &y) {
    bool res = true;
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    double *unorderedResult = new double[A.globalNumberOfRows];
    double *localReorderedResult = new double[A.localNumberOfRows];
    vector<int> localRows;
    for (int i = 0; i < A.globalNumberOfRows; i++) {
        if (rank == A.assign[i]) localRows.push_back(i);
    }
    for (int i = 0; i < A.localNumberOfRows; i++) {
        int idx = lower_bound(localRows.begin(), localRows.end(), A.local2global[i]) - localRows.begin();
        localReorderedResult[idx] = y.values[i];
    }
    int *recvCount = new int[size];
    memset(recvCount, 0, size * sizeof(int));
    for (int i = 0; i < A.globalNumberOfRows; i++) recvCount[A.assign[i]]++;
    int *displs = new int[size+1];
    displs[0] = 0;
    for (int i = 0; i < size; i++) displs[i+1] = displs[i] + recvCount[i];
    MPI_Allgatherv(localReorderedResult, A.localNumberOfRows, MPI_DOUBLE, unorderedResult, recvCount, displs, MPI_DOUBLE, MPI_COMM_WORLD);
    double *result = new double[A.globalNumberOfRows];
    int *index = new int[size];
    memset(index, 0, size * sizeof(int));
    for (int i = 0; i < A.globalNumberOfRows; i++) {
        int p = A.assign[i];
        result[i] = unorderedResult[displs[p] + index[p]];
        index[p]++;
    }
    int nRow, nCol, nNnz;
    vector<Element> elements = GetElementsFromFile(mtxFile, nRow, nCol, nNnz);

    int *ptr = new int[nRow+1];
    int *idx = new int[nNnz];
    double *val = new double[nNnz];
    int p = 0;
    for (int i = 0; i < nNnz; i++) {
        int r = elements[i].row;
        idx[i] = elements[i].col;
        val[i] = elements[i].val;
        //if (rank == 0) printf("%d %d %lf\n", r, idx[i], val[i]);
        while (p <= r) ptr[p++] = i;
    }
    while (p <= nRow) ptr[p++] = nNnz;

    for (int i = 0; i < nRow; i++) {
        double sum = 0;
        for (int j = ptr[i]; j < ptr[i+1]; j++) {
            // TODO
            sum += val[j] *(idx[j] + 1);
            //sum += val[j] * 1;
        }
        double relative_error = abs(abs(result[i] - sum) / result[i]);
        const double EPS = 1e-5;
        if (relative_error > EPS)  {
            if (rank == 0) cerr << "Result is wrong at " << i << " expected value: " << sum << " returned value: " << result[i] << " relative error: " << relative_error << " absolute error: " << abs(result[i]-sum) << endl;
            res = false;
        }
    }
    return res;
}

void DeleteSparseMatrix (SparseMatrix & A) {
}
void DeleteVector (Vector & x) {
}


