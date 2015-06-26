#include <iostream>
#include <algorithm>
#include <mpi.h>
using namespace std;
int rank, size;
const int dataSize = 1000;
const int nIteration = 10;


void comm (double *sendBuffer, double *recvBuffer) {
    MPI_Request *sendRequests = new MPI_Request[size];
    MPI_Request *recvRequests = new MPI_Request[size];


    for (int i = 0; i < size; i++) {
        MPI_Isend(sendBuffer, dataSize, MPI_DOUBLE, i, 112233445, MPI_COMM_WORLD, &sendRequests[i]);
    }
    for (int i = 0; i < size; i++) {
        MPI_Irecv(recvBuffer + i * dataSize, dataSize, MPI_DOUBLE, i, 112233445, MPI_COMM_WORLD, &recvRequests[i]);
    }
    MPI_Status *sendStatuses = new MPI_Status[size];
    MPI_Status *recvStatuses = new MPI_Status[size];
    MPI_Waitall(size, recvRequests, recvStatuses);
    MPI_Waitall(size, sendRequests, sendStatuses);
}
void calc (double *in, double *out) {
    for (int i = 0; i < dataSize * size; i++) {
        out[i] = in[i] * in[i] + in[i] + in[i] * in[i];
    }
}


double GetSynchronizedTime () {
    MPI_Barrier(MPI_COMM_WORLD);
    double t = MPI_Wtime();
    MPI_Bcast(&t, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    return t;
}

double GetBarrieredTime () {
    MPI_Barrier(MPI_COMM_WORLD);
    double t = MPI_Wtime();
    return t;
}
int main (int argc, char **argv) {
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    double sendBuffer[dataSize];
    fill(sendBuffer, sendBuffer + dataSize, rank);
    double recvBuffer[dataSize * size];
    double result[dataSize * size];
    double commTime, calcTime, totalTime;
    // Comm
    {
        for (int k = 0; k < nIteration; k++) {
            double begin = GetSynchronizedTime();
            int nLoop = 1;
            while (GetSynchronizedTime() - begin < 1.0) {
                for (int i = 0; i < nLoop; i++) {
                    comm(sendBuffer, recvBuffer);
                }
                nLoop *= 2;
            }
            double elapsedTime = -GetBarrieredTime();
            for (int i = 0; i < nLoop; i++) {
                comm(sendBuffer, recvBuffer);
            }
            elapsedTime += GetBarrieredTime();
            if (!k) commTime = elapsedTime / nLoop;
            commTime = min(commTime, elapsedTime / nLoop);
        }
    }
    // Calc
    {
        for (int k = 0; k < nIteration; k++) {
            double begin = GetSynchronizedTime();
            int nLoop = 1;
            while (GetSynchronizedTime() - begin < 1.0) {
                for (int i = 0; i < nLoop; i++) {
                    calc(recvBuffer, result);
                }
                nLoop *= 2;
            }
            double elapsedTime = -GetBarrieredTime();
            for (int i = 0; i < nLoop; i++) {
                calc(recvBuffer, result);
            }
            elapsedTime += GetBarrieredTime();
            if (!k) calcTime = elapsedTime / nLoop;
            calcTime = min(calcTime, elapsedTime / nLoop);
        }
    }
    // Total
    {
        for (int k = 0; k < nIteration; k++) {
            double begin = GetSynchronizedTime();
            int nLoop = 1;
            while (GetSynchronizedTime() - begin < 1.0) {
                for (int i = 0; i < nLoop; i++) {
                    comm(sendBuffer, recvBuffer);
                    calc(recvBuffer, result);
                }
                nLoop *= 2;
            }
            double elapsedTime = -GetBarrieredTime();
            for (int i = 0; i < nLoop; i++) {
                comm(sendBuffer, recvBuffer);
                calc(recvBuffer, result);
            }
            elapsedTime += GetBarrieredTime();
            if (!k) totalTime = elapsedTime / nLoop;
            totalTime = min(totalTime, elapsedTime / nLoop);
        }
    }
    if (rank == 0) printf("%.5lf\t%.5lf\t%.5lf\n", commTime, calcTime, totalTime);
    /*for (int i = 0; i < dataSize * size; i++) {
        if (rank == 0) printf("%d\t%.1lf\n", i, result[i]);
    }*/
    MPI_Finalize();
}
