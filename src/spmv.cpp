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
	double* const xv = x.values;
    double *sendBuffer = A.sendBuffer;
#pragma omp parallel for
	for (int i = 0; i < A.totalNumberOfSend; i++) sendBuffer[i] = xv[A.localIndexOfSend[i]];
	//==============================
	// Begin Asynchronouse Communication
	//==============================
	const int MPI_MY_TAG = 141421356;
	MPI_Request *recvRequest = new MPI_Request[A.numberOfRecvNeighbors];
	MPI_Request *sendRequest = new MPI_Request[A.numberOfSendNeighbors];
	double *x_external = (double *) xv + A.localNumberOfRows;
	for (int i = 0; i < A.numberOfRecvNeighbors; i++) {
		int nRecv = A.recvLength[i];
        int src = A.recvNeighbors[i];
		MPI_Irecv(x_external, nRecv, MPI_DOUBLE, src, MPI_MY_TAG, MPI_COMM_WORLD, recvRequest+i);
		x_external += nRecv;
	}
	for (int i = 0; i < A.numberOfSendNeighbors; i++) {
		int nSend = A.sendLength[i];
        int dst = A.sendNeighbors[i];
		MPI_Isend(sendBuffer, nSend, MPI_DOUBLE, dst, MPI_MY_TAG, MPI_COMM_WORLD, &sendRequest[i]);
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
	for (int i = 0; i < A.numberOfRecvNeighbors; i++) {
		MPI_Status status;
		if (MPI_Wait(recvRequest+i, &status)) {
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
	delete [] recvRequest;
	delete [] sendRequest;
	return 0;

}
/*
int SpMV_measurement (const SparseMatrix &A, Vector &x, Vector &y) {
	//==============================
	// Packing
	//==============================
    timingTemp[TIMING_PACKING] -= MPI_Wtime();
    MPI_Barrier(MPI_COMM_WORLD);
	double* const xv = x.values;
    double *sendBuffer = A.sendBuffer;
#pragma omp parallel for
	for (int i = 0; i < A.totalNumberOfSend; i++) sendBuffer[i] = xv[A.localIndexOfSend[i]];
    MPI_Barrier(MPI_COMM_WORLD);
    timingTemp[TIMING_PACKING] += MPI_Wtime();
	//==============================
	// Begin Asynchronouse Communication
	//==============================
    timingTemp[TIMING_TOTAL_COMMUNICATION] -= MPI_Wtime();
	const int MPI_MY_TAG = 141421356;
	MPI_Request *recvRequest = new MPI_Request[A.numberOfRecvNeighbors];
	MPI_Request *sendRequest = new MPI_Request[A.numberOfSendNeighbors];
	double *x_external = (double *) xv + A.localNumberOfRows;
	for (int i = 0; i < A.numberOfRecvNeighbors; i++) {
		int nRecv = A.recvLength[i];
        int src = A.recvNeighbors[i];
		MPI_Irecv(x_external, nRecv, MPI_DOUBLE, src, MPI_MY_TAG, MPI_COMM_WORLD, recvRequest+i);
		x_external += nRecv;
	}
	for (int i = 0; i < A.numberOfSendNeighbors; i++) {
		int nSend = A.sendLength[i];
        int dst = A.sendNeighbors[i];
		MPI_Isend(sendBuffer, nSend, MPI_DOUBLE, dst, MPI_MY_TAG, MPI_COMM_WORLD, &sendRequest[i]);
		sendBuffer += nSend;
	}
    MPI_Barrier(MPI_COMM_WORLD);
    timingTemp[TIMING_TOTAL_COMMUNICATION] += MPI_Wtime();
	//==============================
	// Wait Asynchronous Communication
	//==============================
    timingTemp[TIMING_TOTAL_COMMUNICATION] -= MPI_Wtime();
	for (int i = 0; i < A.numberOfRecvNeighbors; i++) {
		MPI_Status status;
		if (MPI_Wait(recvRequest+i, &status)) {
			std::cerr << "exit in " << __FILE__ << ":" << __LINE__ << std::endl;
			std::exit(-1);
		}
	}
    MPI_Barrier(MPI_COMM_WORLD);
    timingTemp[TIMING_TOTAL_COMMUNICATION] += MPI_Wtime();
	//==============================
	// Compute Internal
	//==============================
    timingTemp[TIMING_INTERNAL_COMPUTATION] -= MPI_Wtime();
	{
		SpMVInternal(A, x, y);
	}
    MPI_Barrier(MPI_COMM_WORLD);
    timingTemp[TIMING_INTERNAL_COMPUTATION] += MPI_Wtime();
	//==============================
	// Compute External
	//==============================
    timingTemp[TIMING_EXTERNAL_COMPUTATION] -= MPI_Wtime();
	{
		SpMVExternal(A, x, y);
	}
    MPI_Barrier(MPI_COMM_WORLD);
    timingTemp[TIMING_EXTERNAL_COMPUTATION] += MPI_Wtime();
	delete [] recvRequest;
	delete [] sendRequest;
	return 0;

}
*/
int SpMV_measurement_once (const SparseMatrix &A, Vector &x, Vector &y) {
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
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
    while (GetSynchronizedTime() - begin < 1.0) {
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
    while (GetSynchronizedTime() - begin < 1.0) {
        fprintf(stderr, "rank %d in\n", rank);
        const int MPI_MY_TAG = 141421356 + nLoop;
        MPI_Request *recvRequest = new MPI_Request[A.numberOfRecvNeighbors];
        MPI_Request *sendRequest = new MPI_Request[A.numberOfSendNeighbors];
        double *x_external = (double *) xv + A.localNumberOfRows;
        sendBuffer = A.sendBuffer;
        for (int i = 0; i < A.numberOfRecvNeighbors; i++) {
            int nRecv = A.recvLength[i];
            int src = A.recvNeighbors[i];
            MPI_Irecv(x_external, nRecv, MPI_DOUBLE, src, MPI_MY_TAG, MPI_COMM_WORLD, recvRequest+i);
            x_external += nRecv;
        }
        fprintf(stderr, "rank %d isend\n", rank);
        for (int i = 0; i < A.numberOfSendNeighbors; i++) {
            int nSend = A.sendLength[i];
            int dst = A.sendNeighbors[i];
            MPI_Isend(sendBuffer, nSend, MPI_DOUBLE, dst, MPI_MY_TAG, MPI_COMM_WORLD, &sendRequest[i]);
            sendBuffer += nSend;
        }
        fprintf(stderr, "rank %d irecv\n", rank);
        for (int i = 0; i < A.numberOfRecvNeighbors; i++) {
            MPI_Status status;
            if (MPI_Wait(recvRequest+i, &status)) {
                std::cerr << "exit in " << __FILE__ << ":" << __LINE__ << std::endl;
                std::exit(-1);
            }
        }
        fprintf(stderr, "rank %d wait\n", rank);
        MPI_Barrier(MPI_COMM_WORLD);
        fprintf(stderr, "rank %d barrier\n", rank);
        delete [] recvRequest;
        delete [] sendRequest;
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
    while (GetSynchronizedTime() - begin < 1.0) {
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
    while (GetSynchronizedTime() - begin < 1.0) {
		SpMVExternal(A, x, y);
        nLoop++;
	}
    elapsedTime += MPI_Wtime();
    timingTemp[TIMING_EXTERNAL_COMPUTATION] = elapsedTime / nLoop;
	return 0;

}
