#include <mpi.h>
#include <ctime>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <omp.h>
#include <mpi.h>
#include "sparse_matrix.h"
#include "vector.h"
#include "spmv.h"
#include "util.h"
#include "mpi_util.h"
#include "timing.h"
#ifdef PRINT_NUMABIND
#include "numa.h"
#endif
using namespace std;

vector<char*>   timingDetail(NUMBER_OF_TIMING, NULL);
vector<double>  timing(NUMBER_OF_TIMING);
vector<double>  timingTemp(NUMBER_OF_TIMING);

#define PERR(s)   if (rank == 0) fprintf(stderr, "%s", s);
#define POUT(s)   if (rank == 0) fprintf(stdout, "%s", s);

int main (int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s <prefix of part file (i.e. 'partition/test.mtx')> [matrix file (to verify)]\n", argv[0]);
        exit(1);
    }
    string mtxFile;
    bool verify = false;
    if (argc == 3) {
        verify = true;
        mtxFile = argv[2];
    }
    string partName = argv[1];
    string mtxName = GetBasename(argv[1]);
    MPI_Init(&argc, &argv);

    //------------------------------
    // INIT
    //------------------------------
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    if (rank == 0) fprintf(stderr, "Begin %s\n", mtxName.c_str());
    string partFile = string(argv[1]) + "-" + to_string(static_cast<long long>(size)) + "-" + to_string(static_cast<long long>(rank)) + ".part"; 
    PERR("Loading sparse matrix and vector ... ");
#ifdef GPU
    SelectDevice();
#endif
    MPI_Barrier(MPI_COMM_WORLD); fflush(stderr); fflush(stdout);
    SparseMatrix A;
    Vector x, y;
    LoadInput(partFile, A, x);
#ifdef USE_DENSE_INTERNAL_INDEX
    CreateDenseInternalIdx(A, x);
#endif
    CreateZeroVector(y, A.localNumberOfRows);
    MPI_Barrier(MPI_COMM_WORLD); fflush(stderr); fflush(stdout);
    PERR("done\n");

    //------------------------------
    // SpMV (Count loop number)
    //------------------------------
    int nLoop = 1;
    {
        double begin = GetSynchronizedTime();
        while (GetSynchronizedTime() - begin < THRESHOLD_SECOND)  {
            for (int l = 0; l < nLoop; l++) SpMV(A, x, y);
            nLoop *= 2;
        }
    }

    //------------------------------
    // SpMV (Asynchronous)
    //------------------------------
    PERR("Computing SpMV ... ");
    timingDetail[TIMING_TOTAL_SPMV] = "TotalSpMV";
    for (int i = 0; i < NUMBER_OF_LOOP_OF_SPMV; i++) {
        fill(timingTemp.begin(), timingTemp.end(), 0);
        double elapsedTime = -GetBarrieredTime();
        for (int l = 0; l < nLoop; l++) {
            SpMV(A, x, y);
        }
        elapsedTime += GetBarrieredTime();
        if (!i || timing[TIMING_TOTAL_SPMV] > elapsedTime / nLoop) {
            timing[TIMING_TOTAL_SPMV] = elapsedTime / nLoop;
        }
    }
    PERR("done\n");

    //------------------------------
    // Verify
    //------------------------------
    if (verify) {
        fill(y.values, y.values + y.localLength, 0);
        SpMV(A, x, y);
        PERR("Verifying ... ");
        VerifySpMV(mtxFile, A, y);
        MPI_Barrier(MPI_COMM_WORLD); fflush(stderr); fflush(stdout);
        PERR("done\n");

        //------------------------------
        // Print 
        //------------------------------
#ifdef PRINT_RESULT
        PERR("Printing result ... ");
        PrintResult(A, y);
        MPI_Barrier(MPI_COMM_WORLD); fflush(stderr); fflush(stdout);
        PERR("done\n");
#endif
    }

    //------------------------------
    // SpMV_measure (Synchronous)
    //------------------------------
    PERR("Computing SpMV_measurement_once ... ");
    timingDetail[TIMING_TOTAL_COMMUNICATION] = "TotalCommunication";
    timingDetail[TIMING_TOTAL_COMPUTATION]  = "TotalComputation";
    timingDetail[TIMING_INTERNAL_COMPUTATION]  = "InternalComputation";
    timingDetail[TIMING_EXTERNAL_COMPUTATION]  = "ExternalComputation";
    timingDetail[TIMING_PACKING] = "Packing";
    for (int i = 0; i < NUMBER_OF_LOOP_OF_MEASURENT_SPMV; i++) {
        SpMV_measurement_once(A, x, y);
        if (!i) {
            timing[TIMING_TOTAL_COMMUNICATION] = timingTemp[TIMING_TOTAL_COMMUNICATION];
            timing[TIMING_INTERNAL_COMPUTATION] = timingTemp[TIMING_INTERNAL_COMPUTATION];
            timing[TIMING_EXTERNAL_COMPUTATION] = timingTemp[TIMING_EXTERNAL_COMPUTATION];
            timing[TIMING_PACKING] = timingTemp[TIMING_PACKING];
            timing[TIMING_TOTAL_COMPUTATION] = timingTemp[TIMING_INTERNAL_COMPUTATION] + 
                timingTemp[TIMING_EXTERNAL_COMPUTATION];
        } else {
            amin(timing[TIMING_TOTAL_COMMUNICATION], timingTemp[TIMING_TOTAL_COMMUNICATION]);
            amin(timing[TIMING_INTERNAL_COMPUTATION], timingTemp[TIMING_INTERNAL_COMPUTATION]);
            amin(timing[TIMING_EXTERNAL_COMPUTATION], timingTemp[TIMING_EXTERNAL_COMPUTATION]);
            amin(timing[TIMING_PACKING], timingTemp[TIMING_PACKING]);
            amin(timing[TIMING_TOTAL_COMPUTATION], timingTemp[TIMING_INTERNAL_COMPUTATION] + 
                    timingTemp[TIMING_EXTERNAL_COMPUTATION]);
        }
    }
    PERR("done\n");

    /*
    //------------------------------
    // DELETE
    //------------------------------
    PERR("Deleting A, x ... ");
    DeleteSparseMatrix(A);
    DeleteVector(x);
    MPI_Barrier(MPI_COMM_WORLD); fflush(stderr); fflush(stdout);
    PERR("done\n");
    PERR("Deleting y ... ");
    DeleteVector(y);
    MPI_Barrier(MPI_COMM_WORLD); fflush(stderr); fflush(stdout);
    PERR("done\n");
    */

    //------------------------------
    // REPORT
    //------------------------------
    PERR("Reporting ... ");
    POUT("++++++++++++++++++++++++++++++++++++++++\n");
#ifdef PRINT_HOSTNAME
    PrintHostName();
#endif

    PrintOption();
    if (rank == 0) {
        printf("%25s\t%s\n", "Matrix", mtxName.c_str());
        printf("%25s\t%d\n", "NumberOfProcesses", size);
        printf("%25s\t%d\n", "NumberOfThreads",  omp_get_max_threads());
#ifdef PRINT_NUMABIND
        if (numa_available() != -1) {
            printf("%25s\t%d\n", "RunNodeBindMask", (int)*numa_get_run_node_mask()->maskp);
            printf("%25s\t%d\n", "MemBindMask", (int)*numa_get_membind()->maskp);
        }
#endif
        printf("%25s\t%d\n", "NumberOfRows", A.globalNumberOfRows);
        printf("%25s\t%d\n", "NumberOfNonzeros", A.globalNumberOfNonzeros);
#ifdef PRINT_PERFORMANCE
        printf("%25s\t%.10lf\n", "GFLOPS", A.globalNumberOfNonzeros * 2 / timing[TIMING_TOTAL_SPMV] / 1e9);
        printf("%25s\t%d\n", "nLoop", nLoop);
        for (int i = 0; i < NUMBER_OF_TIMING; i++) {
            if (timingDetail[i] != NULL) {
                printf("%25s\t%.10lf\n", timingDetail[i], timing[i]);
            }
        }
#endif
    }
    POUT("----------------------------------------\n");
    PERR("done\n");
    PERR("Finalizing ... ");
    MPI_Finalize();
    PERR("done\n");
    PERR("Complete!!\n");
    return 0;
}
