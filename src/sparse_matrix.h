#pragma once
#include <map>
struct SparseMatrix {
    int *assign;
    int globalNumberOfRows;
    int globalNumberOfNonzeros;
    int localNumberOfRows;
    int localNumberOfNonzeros;
    //==============================
    // Asynchronous Communication
    //==============================
    int *internalPtr;
    int *internalIdx;
    double *internalVal;
    int *externalPtr;
    int *externalIdx;
    double *externalVal;

    int totalNumberOfUsedCols;
    int *local2global;
    std::map<int, int> global2local;

    int numberOfSendNeighbors;
    int numberOfRecvNeighbors;    
    int *sendNeighbors;
    int *recvNeighbors;

    int *recvLength;
    int *sendLength;
    int totalNumberOfSend;
    int totalNumberOfRecv;
    int *localIndexOfSend;
    int *localIndexOfRecv;
    double *sendBuffer;
};
