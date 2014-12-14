#pragma once
struct SparseMatrix {
    global_int_t globalNumberOfRows;
    local_int_t localNumberOfRows;
    global_int_t globalNumberOfNonzeros;
    local_int_t localNumberOfNonzeros;
    local_int_t *numberOfNonzerosInRow;

    double *values;
    global_int_t *globalIndices;
    local_int_t  *localIndices;
    
    int numberOfSendNeighbors;
    int numberOfReceiveNeighbors;    
    int *sendNeighbors;
    int *receiveNeighbors;

    local_int_t *receiveLength;
    local_int_t *sendLength;
    local_int_t totalNumberOfSend;
    local_int_t totalNumberOfReceive;
    local_int_t localIndexOfSend;
    double *sendBuffer;

    void *optimizationData;
};
