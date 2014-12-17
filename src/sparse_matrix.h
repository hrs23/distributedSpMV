#pragma once
struct SparseMatrix {
    int *assignment;

    int globalNumberOfRows;
    int localNumberOfRows;
    int globalNumberOfNonzeros;
    int localNumberOfNonzeros;
    int *numberOfNonzerosInRow;

    double *values;
    int *globalIndices;
    int  *localIndices;
    
    int numberOfSendNeighbors;
    int numberOfReceiveNeighbors;    
    int *sendNeighbors;
    int *receiveNeighbors;

    int *receiveLength;
    int *sendLength;
    int totalNumberOfSend;
    int totalNumberOfReceive;
    int localIndexOfSend;
    double *sendBuffer;

    void *optimizationData;
};
