#include <vector>
using namespace std;
#define NUMBER_OF_LOOP_OF_SPMV      10
#define NUMBER_OF_TIMING            20

#define TIMING_TOTAL_SPMV                   0
#define TIMING_REAL_PACKING                 1
#define TIMING_REAL_BEGIN_COMMUNICATION     2
#define TIMING_REAL_INTERNAL_COMPUTATION    3
#define TIMING_REAL_EXTERNAL_COMPUTATION    4
#define TIMING_REAL_WAIT_COMMUNICATION      5
#define TIMING_TOTAL_COMMUNICATION          6
#define TIMING_TOTAL_COMPUTATION            7
#define TIMING_PACKING                      8
#define TIMING_INTERNAL_COMPUTATION         9
#define TIMING_EXTERNAL_COMPUTATION         10

extern vector<double> timingTemp;
