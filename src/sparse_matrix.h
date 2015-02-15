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


#ifdef GPU
    int *cuda_internalPtr;
    int *cuda_internalIdx;
    double *cuda_internalVal;
    int *cuda_externalPtr;
    int *cuda_externalIdx;
    double *cuda_externalVal;
    double *cuda_x_values;
    double *cuda_y_values;

#endif
};
