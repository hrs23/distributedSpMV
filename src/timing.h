#include <vector>
using namespace std;
#define NUMBER_OF_LOOP_OF_SPMV      10
#define NUMBER_OF_LOOP_OF_MEASURENT_SPMV      5
#define NUMBER_OF_TIMING            20

#define TIMING_TOTAL_SPMV                   0
#define TIMING_REAL_PACKING                 1
#define TIMING_REAL_BEGIN_COMMUNICATION     2
#define TIMING_REAL_INTERNAL_COMPUTATION    3
#define TIMING_REAL_EXTERNAL_COMPUTATION    4
#define TIMING_REAL_WAIT_COMMUNICATION      5
#define TIMING_REAL_DELETE                  6

#define TIMING_TOTAL_COMMUNICATION          10
#define TIMING_TOTAL_COMPUTATION            11
#define TIMING_PACKING                      12
#define TIMING_INTERNAL_COMPUTATION         13
#define TIMING_EXTERNAL_COMPUTATION         14

extern vector<double> timingTemp;
