#include <mpi.h>
#include <iostream>
#include "spmv.h"
#include "sparse_matrix.h"
#include "vector.h"
#include "spmv_kernel.h"
using namespace std;
int SpMV (const SparseMatrix &A, Vector &x, Vector &y) {
	int size, rank;
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	double* const xv = x.values;
	//==============================
	// Packing
	//==============================
    double *sendBuffer = A.sendBuffer;
#pragma omp for
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
	MPI_Barrier(MPI_COMM_WORLD);
	delete [] recvRequest;
	delete [] sendRequest;
	return 0;

}
