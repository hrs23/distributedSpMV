# CMake variable
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
# -- HPCS --
#SET(CMAKE_CXX_COMPILER "/opt/intel/impi/5.0.1.035/intel64/bin/mpiicpc") 
# -- COMA --
#SET(CMAKE_CXX_COMPILER "/app/intel/impi/4.1.3.048/intel64/bin/mpiicpc") 
SET(CMAKE_CXX_COMPILER "mpiicpc") 

SET(CMAKE_CXX_FLAGS_RELEASE "-xHOST -O3 -ipo -mkl -fopenmp -std=c++11 -Wall")
#SET(CMAKE_CXX_FLAGS_RELEASE "-mmic -O3 -ipo -mkl -fopenmp -std=c++11 -Wall")

SET(CMAKE_CXX_FLAGS_DEBUG "-mkl -fopenmp -std=c++11 -Wall -g")
SET(CMAKE_BUILD_TYPE RELEASE)
PROJECT(distributedSpMV)

# User variable
SET(INCLUDE_PATH "include")
SET(LIBRARY_PATH "lib")
SET(BINARY_PATH bin)

SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BINARY_PATH})
INCLUDE_DIRECTORIES(${INCLUDE_PATH})
LINK_DIRECTORIES(${LIBRARY_PATH})

# TARGET : partition
SET(TARGET partition)
SET(SOURCE  
    src/partition.cpp
    src/util.cpp
    )
SET(LIBRARY patoh)
ADD_EXECUTABLE(${TARGET} ${SOURCE})
TARGET_LINK_LIBRARIES(${TARGET} ${LIBRARY})

# TARGET : spmv
SET(TARGET spmv)
SET(SOURCE 
    src/main.cpp
    src/util.cpp
    src/mpi_util.cpp
    src/spmv.cpp
    src/spmv_kernel.cpp
    )
ADD_EXECUTABLE(${TARGET} ${SOURCE})
