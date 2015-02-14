#include <mpi.h>
#include <iostream>
#include <vector>
#include "spmv.h"
#include "sparse_matrix.h"
#include "vector.h"
#include "spmv_kernel.h"
#include "mpi_util.h"
#include "timing.h"
using namespace std;
int SpMV (const SparseMatrix &A, Vector &x, Vector &y) {
	//==============================
	// Packing
	//==============================
	double *xv = x.values;
    double *sendBuffer = A.sendBuffer;
#pragma omp parallel for
	for (int i = 0; i < A.totalNumberOfSend; i++) sendBuffer[i] = xv[A.localIndexOfSend[i]];
	//==============================
	// Begin Asynchronouse Communication
	//==============================
	const int MPI_MY_TAG = 141421356;
	MPI_Request *recvRequests = new MPI_Request[A.numberOfRecvNeighbors];
	MPI_Request *sendRequests = new MPI_Request[A.numberOfSendNeighbors];
	double *x_external = (double *) xv + A.localNumberOfRows;
	for (int i = 0; i < A.numberOfRecvNeighbors; i++) {
		int nRecv = A.recvLength[i];
        int src = A.recvNeighbors[i];
		MPI_Irecv(x_external, nRecv, MPI_DOUBLE, src, MPI_MY_TAG, MPI_COMM_WORLD, &recvRequests[i]);
		x_external += nRecv;
	}
	for (int i = 0; i < A.numberOfSendNeighbors; i++) {
		int nSend = A.sendLength[i];
        int dst = A.sendNeighbors[i];
		MPI_Isend(sendBuffer, nSend, MPI_DOUBLE, dst, MPI_MY_TAG, MPI_COMM_WORLD, &sendRequests[i]);
		sendBuffer += nSend;
	}
	//==============================
	// Compute Internal
	//==============================
	{
		SpMVInternal(A, x, y);
	}
	//==============================
    // Wait Asynchronous Communication
    //==============================
    MPI_Status *recvStatuses = new MPI_Status[A.numberOfRecvNeighbors];
    if (A.numberOfRecvNeighbors) {
        if (MPI_Waitall(A.numberOfRecvNeighbors, recvRequests, recvStatuses)) {
            std::cerr << "exit in SpMV.hpp:" << __LINE__ << std::endl;
            std::exit(-1);
        }
    }
    //==============================
    // Compute External
    //==============================
    {
        SpMVExternal(A, x, y);
    }

	//==============================
    // Wait Asynchronous Communication
    //==============================
    MPI_Status *sendStatuses = new MPI_Status[A.numberOfSendNeighbors];
    if (A.numberOfSendNeighbors) {
        if (MPI_Waitall(A.numberOfSendNeighbors, sendRequests, sendStatuses)) {
            std::cerr << "exit in SpMV.hpp:" << __LINE__ << std::endl;
            std::exit(-1);
        }
    }
    delete [] recvRequests;
    delete [] sendRequests;
    delete [] recvStatuses;
    delete [] sendStatuses;
    return 0;

}
int SpMV_measurement_once (const SparseMatrix &A, Vector &x, Vector &y) {
    double* const xv = x.values;
    double *sendBuffer = A.sendBuffer;
    //==============================
    // Packing
    //==============================
    double begin, elapsedTime;
    int nLoop;
    begin = GetSynchronizedTime();
    elapsedTime = -MPI_Wtime();
    nLoop = 0;
    while (GetSynchronizedTime() - begin < 10.0) {
#pragma omp parallel for
        for (int i = 0; i < A.totalNumberOfSend; i++) sendBuffer[i] = xv[A.localIndexOfSend[i]];
        nLoop++;
    }
    elapsedTime += MPI_Wtime();
    timingTemp[TIMING_PACKING] = elapsedTime / nLoop;
    MPI_Barrier(MPI_COMM_WORLD);
    //==============================
    // Begin synchronouse communication
    //==============================

    begin = GetSynchronizedTime();
    elapsedTime = -MPI_Wtime();
    nLoop = 0;
    while (GetSynchronizedTime() - begin < 10.0) {
        const int MPI_MY_TAG = 141421356 + nLoop;
        MPI_Request *recvRequests = new MPI_Request[A.numberOfRecvNeighbors];
        MPI_Request *sendRequests = new MPI_Request[A.numberOfSendNeighbors];
        double *x_external = (double *) xv + A.localNumberOfRows;
        sendBuffer = A.sendBuffer;
        for (int i = 0; i < A.numberOfRecvNeighbors; i++) {
            int nRecv = A.recvLength[i];
            int src = A.recvNeighbors[i];
            MPI_Irecv(x_external, nRecv, MPI_DOUBLE, src, MPI_MY_TAG, MPI_COMM_WORLD, &recvRequests[i]);
            x_external += nRecv;
        }
        for (int i = 0; i < A.numberOfSendNeighbors; i++) {
            int nSend = A.sendLength[i];
            int dst = A.sendNeighbors[i];
            MPI_Isend(sendBuffer, nSend, MPI_DOUBLE, dst, MPI_MY_TAG, MPI_COMM_WORLD, &sendRequests[i]);
            sendBuffer += nSend;
        }
        MPI_Status *recvStatuses = new MPI_Status[A.numberOfRecvNeighbors];
        if (A.numberOfRecvNeighbors) {
            if (MPI_Waitall(A.numberOfRecvNeighbors, recvRequests, recvStatuses)) {
                std::cerr << "exit in SpMV.hpp:" << __LINE__ << std::endl;
                std::exit(-1);
            }
        }
        MPI_Status *sendStatuses = new MPI_Status[A.numberOfSendNeighbors];
        if (A.numberOfSendNeighbors) {
            if (MPI_Waitall(A.numberOfSendNeighbors, sendRequests, sendStatuses)) {
                std::cerr << "exit in SpMV.hpp:" << __LINE__ << std::endl;
                std::exit(-1);
            }
        }
        delete [] recvRequests;
        delete [] sendRequests;
        delete [] recvStatuses;
        delete [] sendStatuses;
        nLoop++;
    }
    elapsedTime += MPI_Wtime();
    timingTemp[TIMING_TOTAL_COMMUNICATION] = elapsedTime / nLoop;

    //==============================
    // Compute Internal
    //==============================
    begin = GetSynchronizedTime();
    elapsedTime = -MPI_Wtime();
    nLoop = 0;
    while (GetSynchronizedTime() - begin < 10.0) {
        SpMVInternal(A, x, y);
        nLoop++;
    }
    elapsedTime += MPI_Wtime();
    timingTemp[TIMING_INTERNAL_COMPUTATION] = elapsedTime / nLoop;
    //==============================
    // Compute External
    //==============================

    begin = GetSynchronizedTime();
    elapsedTime = -MPI_Wtime();
    nLoop = 0;
    while (GetSynchronizedTime() - begin < 10.0) {
        SpMVExternal(A, x, y);
        nLoop++;
    }
    elapsedTime += MPI_Wtime();
    timingTemp[TIMING_EXTERNAL_COMPUTATION] = elapsedTime / nLoop;
    return 0;

}
