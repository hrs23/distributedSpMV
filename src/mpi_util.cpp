#include <string>
#include <iostream>
#include <fstream>
#include <map>
#include <cassert>
#include <cstring>
#include <cstdio>
#include <mpi.h>
#include "mpi_util.h"
#include "sparse_matrix.h"
#include "vector.h"
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
            if (rank == 0) cerr << "internal " << row << " " << col << " " << val << " i & ip " << i << " " << ip << endl;
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
            if (rank == 0) cerr << "external " << row << " " << col << " " << val << endl;
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
    fill(x.values, x.values + A.totalNumberOfUsedCols, 1);
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

void DeleteSparseMatrix (SparseMatrix & A) {
}
void DeleteVector (Vector & x) {
}


