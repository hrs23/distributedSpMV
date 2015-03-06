#include <iostream>
#include <algorithm>
#include <mpi.h>
using namespace std;
int rank, size;
const int dataSize = 100;


void comm (dobule *sendBuffer, double *recvBuffer) {
    MPI_Requset *sendRequest = new MPI_Request[size];
    MPI_Requset *recvRequest = new MPI_Request[size];
    for (int i = 0; i < size; i++) {
        MPI_Isend(sendBuffer, dataSize, MPI_DOUBLE, i, 112233445, MPI_COMM_WORLD, sendRequest[i]);
    }
    for (int i = 0; i < size; i++) {
        MPI_IRecv(recvBuffer + i * dataSize, dataSize, MPI_DOUBLE, i, 112233445, MPI_COMM_WORLD, recvRequest[i]);
    }
    MPI_Status *sendStatuses = new MPI_Status[size];
    MPI_Status *recvStatuses = new MPI_Status[size];
    MPI_Waitall(size, recvReuests, recvStatuses);
    MPI_Waitall(size, sendReuests, sendStatuses);
}
void calc (double *in, double *out) {
    for (int i = 0; i < dataSize * size; i++) {
        for (int j = 0; j < dataSize * size; j++) {
            out[i] = in[i] * in[j] + in[j];
        }
    }
}
int main (int argc, char **argv) {
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    double sendBuffer[dataSize];
    fill(sendBuffer, sendBuffer + dataSize, rank);
    double recvBuffer[dataSize * size];
    comm(sendBuffer, recvBuffer);
    calc(recvBuffer, result);
    for (int i = 0; i < dataSize * size; i++) {
        if (rank == 0) printf("%d\t%.1lf\n", i, result[i]);
    }
    MPI_Finalize();
}
