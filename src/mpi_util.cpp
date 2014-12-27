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
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    int nCol;
    ifs >> A.globalNumberOfRows >> nCol >> A.globalNumberOfNonzeros;
    assert(A.globalNumberOfRows == nCol);
    A.assign = new int[nRow];
    for (int i = 0; i < nRow; i++) ifs >> A.assign[i];
    ifs >> A.localNumberOfRows >> A.localNumberOfNonzeros;

    int numInternalNnz;
    ifs >> numInternalNnz;
    A.localNumberOfNonzeros = numInternalNnz + numExternalNnz;
    A.internalPtr = new int[A.localNumberOfRows + 1];
    A.internalIdx = new int[numInternalNnz];
    A.internalVal = new double[numInternalNnz];
    {
        int ip = 0;
        for (int i = 0; i < numInternalNnz; i++) {
            int row, col;
            double val;
            ifs >> row >> col >> val;
            A.internalIdx[ip] = col;
            A.internalVal[ip] = val;
            while (ip <= row) A.internalPtr[ip++] = i;
        }
        while (ip <= A.localNumberOfRows) A.internalPtr[ip++] = numInternalNnz;
    }
    int numExternalNnz;
    ifs >> numExternalNnz;
    A.externalPtr = new int[A.localNumberOfRows + 1];
    A.externalIdx = new int[numExternalNnz];
    A.externalVal = new double[numExternalNnz];
    {
        int ep = 0;
        for (int i = 0; i < numExternalNnz; i++) {
            int row, col;
            double val;
            ifs >> row >> col >> val;
            while (ep <= row) A.internalPtr[ep++] = i;
        }
        while (ep <= A.localNumberOfRows) A.externalPtr[ep++] = numExternalNnz;
    }
    ifs >> A.totalNumberOfUsedCol;
    A.local2global = new int[totalNumberOfUsedCol];
    for (int i = 0; i < totalNumberOfUsedCols; i++) {
        ifs >> A.local2global[i];
    }
    ifs >> A.numberOfSendNeighbors >> A.totalNumberOfSend;
    A.sendLength = new int[A.numberOfSendNeighbors];
    A.sendNeighbors = new int[A.numberOfSendNeighbors];
    A.sendBuffer = new double[A.totalNumberOfSend];
    int sendOffset = 0;
    for (int i = 0; i < A.numberOfSendNeighbors; i++) {
        ifs >> A.sendNeighbors[i] >> A.sendLength[i];
        for (int j = 0; j < A.sendLength[i]; j++) {
            ifs >> A.localIndexOfSend[sendOffset + j];
        }
        sendOffset += A.sendLength[i];
    }
    assert(sendOffset == A.totalNumberOfSend);
    ifs >> A.numberOfRecvNeighbors >> A.totalNumberOfRecv;
    A.recvLength = new int[A.numberOfRecvNeighbors];
    A.recvNeighbors = new int[A.numberOfRecvNeighbors];
    int recvOffset = 0;
    for (int i = 0; i < A.numberOfRecvNeighbors; i++) {
        ifs >> A.recvNeighbors[i] >> A.recvLength[i];
        for (int j = 0; j < A.recvLength[i]; j++) {
            ifs >> A.localIndexOfRecv[recvOffset + j];
        }
        recvOffset += A.recvLength[i];
    }
    assert(recvOffset == A.totalNumberOfRecv);
}
