#pragma once
struct SparseMatrix {
    int *assign;

    int globalNumberOfRows;
    int localNumberOfRows;
    int globalNumberOfNonzeros;
    int localNumberOfNonzeros;

    int numberOfSendNeighbors;
    int numberOfRecvNeighbors;    
    int *sendNeighbors;
    int *recvNeighbors;

    int *recvLength;
    int *sendLength;
    int totalNumberOfSend;
    int totalNumberOfRecv;
    int *localIndexOfSend;
    int *localindexOfRecv;
    double *sendBuffer;

    //==============================
    // Asynchronous Communication
    //==============================
    int *localPtr;
    int *localIdx;
    double *localVal;
    int *externalPtr;
    int *externalIdx;
    double *externalVal;

    //==============================
    // Multithread MPI call
    //==============================
    int *sendPointer;
    int *recvPointer;
};
