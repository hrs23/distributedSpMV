#include <vector>
using namespace std;
#define NUMBER_OF_LOOP_OF_SPMV      10
#define NUMBER_OF_LOOP_OF_MEASURENT_SPMV      5
#define NUMBER_OF_TIMING            20

#ifndef THRESHOLD_SECOND
#define THRESHOLD_SECOND            1.0
#endif

#define TIMING_TOTAL_SPMV                   0

#define TIMING_TOTAL_COMMUNICATION          10
#define TIMING_TOTAL_COMPUTATION            11
#define TIMING_PACKING                      12
#define TIMING_INTERNAL_COMPUTATION         13
#define TIMING_EXTERNAL_COMPUTATION         14

extern vector<double> timingTemp;
