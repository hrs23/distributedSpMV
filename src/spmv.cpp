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
    timingTemp[TIMING_REAL_PACKING] -= GetSynchronizedTime();
	double *xv = x.values;
    double *sendBuffer = A.sendBuffer;
#pragma omp parallel for
	for (int i = 0; i < A.totalNumberOfSend; i++) sendBuffer[i] = xv[A.localIndexOfSend[i]];
    timingTemp[TIMING_REAL_PACKING] += GetSynchronizedTime();
	//==============================
	// Begin Asynchronouse Communication
	//==============================
    timingTemp[TIMING_REAL_BEGIN_COMMUNICATION] -= GetSynchronizedTime();
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
    timingTemp[TIMING_REAL_BEGIN_COMMUNICATION] += GetSynchronizedTime();
	//==============================
	// Compute Internal
	//==============================
    timingTemp[TIMING_REAL_INTERNAL_COMPUTATION] -= GetSynchronizedTime();
	{
		SpMVInternal(A, x, y);
	}
    timingTemp[TIMING_REAL_INTERNAL_COMPUTATION] += GetSynchronizedTime();
	//==============================
    // Wait Asynchronous Communication
    //==============================
    timingTemp[TIMING_REAL_WAIT_COMMUNICATION] -= GetSynchronizedTime();
    MPI_Status *recvStatuses = new MPI_Status[A.numberOfRecvNeighbors];
    if (A.numberOfRecvNeighbors) {
        if (MPI_Waitall(A.numberOfRecvNeighbors, recvRequests, recvStatuses)) {
            std::cerr << "exit in SpMV.hpp:" << __LINE__ << std::endl;
            std::exit(-1);
        }
    }
    timingTemp[TIMING_REAL_WAIT_COMMUNICATION] += GetSynchronizedTime();
    //==============================
    // Compute External
    //==============================
    timingTemp[TIMING_REAL_EXTERNAL_COMPUTATION] -= GetSynchronizedTime();
    {
        SpMVExternal(A, x, y);
    }
    timingTemp[TIMING_REAL_EXTERNAL_COMPUTATION] += GetSynchronizedTime();

	//==============================
    // Wait Asynchronous Communication
    //==============================
    timingTemp[TIMING_REAL_WAIT_COMMUNICATION] -= GetSynchronizedTime();
    MPI_Status *sendStatuses = new MPI_Status[A.numberOfSendNeighbors];
    if (A.numberOfSendNeighbors) {
        if (MPI_Waitall(A.numberOfSendNeighbors, sendRequests, sendStatuses)) {
            std::cerr << "exit in SpMV.hpp:" << __LINE__ << std::endl;
            std::exit(-1);
        }
    }
    timingTemp[TIMING_REAL_WAIT_COMMUNICATION] += GetSynchronizedTime();
    timingTemp[TIMING_REAL_DELETE] -= GetSynchronizedTime();
    delete [] recvRequests;
    delete [] sendRequests;
    delete [] recvStatuses;
    delete [] sendStatuses;
    timingTemp[TIMING_REAL_DELETE] += GetSynchronizedTime();
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
    elapsedTime = -GetSynchronizedTime();
    nLoop = 0;
#ifdef MIC
    while (GetSynchronizedTime() - begin < 1.0) {
#endif
#pragma omp parallel for
        for (int i = 0; i < A.totalNumberOfSend; i++) sendBuffer[i] = xv[A.localIndexOfSend[i]];
        nLoop++;
#ifdef MIC
    }
#endif
    elapsedTime += GetSynchronizedTime();
    timingTemp[TIMING_PACKING] = elapsedTime / nLoop;
    //==============================
    // Begin synchronouse communication
    //==============================

    begin = GetSynchronizedTime();
    elapsedTime = -GetSynchronizedTime();
    nLoop = 0;
#ifdef MIC
    while (GetSynchronizedTime() - begin < 1.0) {
#endif
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
#ifdef MIC
    }
#endif
    elapsedTime += GetSynchronizedTime();
    timingTemp[TIMING_TOTAL_COMMUNICATION] = elapsedTime / nLoop;

    //==============================
    // Compute Internal
    //==============================
    begin = GetSynchronizedTime();
    elapsedTime = -GetSynchronizedTime();
    nLoop = 0;
#ifdef MIC
    while (GetSynchronizedTime() - begin < 1.0) {
#endif
        SpMVInternal(A, x, y);
        nLoop++;
#ifdef MIC
    }
#endif
    elapsedTime += GetSynchronizedTime();
    timingTemp[TIMING_INTERNAL_COMPUTATION] = elapsedTime / nLoop;
    //==============================
    // Compute External
    //==============================

    begin = GetSynchronizedTime();
    elapsedTime = -GetSynchronizedTime();
    nLoop = 0;
#ifdef MIC
    while (GetSynchronizedTime() - begin < 1.0) {
#endif
        SpMVExternal(A, x, y);
        nLoop++;
#ifdef MIC
    }
#endif
    elapsedTime += GetSynchronizedTime();
    timingTemp[TIMING_EXTERNAL_COMPUTATION] = elapsedTime / nLoop;
    return 0;

}
