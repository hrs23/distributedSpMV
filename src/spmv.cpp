#include <mpi.h>
#include <iostream>
#include <vector>
#include "spmv.h"
#include "sparse_matrix.h"
#include "vector.h"
#include "spmv_kernel.h"
#include "timing.h"
using namespace std;
int SpMV (const SparseMatrix &A, Vector &x, Vector &y) {
    /*
	int size, rank;
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    */
	//==============================
	// Packing
	//==============================
    timingTemp[TIMING_PACKING] = -MPI_Wtime();
    MPI_Barrier(MPI_COMM_WORLD);
	double* const xv = x.values;
    double *sendBuffer = A.sendBuffer;
#pragma omp for
	for (int i = 0; i < A.totalNumberOfSend; i++) sendBuffer[i] = xv[A.localIndexOfSend[i]];
    MPI_Barrier(MPI_COMM_WORLD);
    timingTemp[TIMING_PACKING] += MPI_Wtime();
	//==============================
	// Begin Asynchronouse Communication
	//==============================
    timingTemp[TIMING_TOTAL_COMMUNICATION] = -MPI_Wtime();
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
	// Compute Internal
	//==============================
    timingTemp[TIMING_TOTAL_COMPUTATION] = -MPI_Wtime();
	{
		SpMVInternal(A, x, y);
	}
    MPI_Barrier(MPI_COMM_WORLD);
    timingTemp[TIMING_TOTAL_COMPUTATION] += MPI_Wtime();
	//==============================
	// Wait Asynchronous Communication
	//==============================
    timingTemp[TIMING_TOTAL_COMMUNICATION] -= MPI_Wtime();
	for (int i = 0; i < A.numberOfRecvNeighbors; i++) {
		MPI_Status status;
		if (MPI_Wait(recvRequest+i, &status)) {
			std::cerr << "exit in SpMV.hpp:" << __LINE__ << std::endl;
			std::exit(-1);
		}
	}
    MPI_Barrier(MPI_COMM_WORLD);
    timingTemp[TIMING_TOTAL_COMMUNICATION] += MPI_Wtime();
	//==============================
	// Compute External
	//==============================
    timingTemp[TIMING_TOTAL_COMPUTATION] -= MPI_Wtime();
	{
		SpMVExternal(A, x, y);
	}
    MPI_Barrier(MPI_COMM_WORLD);
    timingTemp[TIMING_TOTAL_COMPUTATION] += MPI_Wtime();
	delete [] recvRequest;
	delete [] sendRequest;
	return 0;

}
