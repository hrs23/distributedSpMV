SOURCE_DIR = src
BINARY_DIR = bin
OBJECT_DIR = obj
LIBRARY_DIR = lib
INCLUDE_DIR = include

CXX = mpiicpc
LDFLAGS = -L$(LIBRARY_DIR) -L$(OBJECT_DIR)
CXXFLAGS = -std=c++11 -ipo -Wall -O3 -mkl -fopenmp -I$(INCLUDE_DIR)

vpath %.cpp $(SOURCE_DIR)
partition_sources = partition.cpp util.cpp
spmv_sources = main.cpp mpi_util.cpp spmv.cpp spmv_kernel.cpp util.cpp 

partition_objects = $(addprefix $(OBJECT_DIR)/, $(partition_sources:.cpp=.o))
spmv_objects_cpu = $(addprefix $(OBJECT_DIR)/, $(spmv_sources:.cpp=.o))
spmv_objects_mic = $(addprefix $(OBJECT_DIR)/, $(spmv_sources:.cpp=.o.mic))

SPMV_MIC=$(BINARY_DIR)/spmv.mic
SPMV_CPU=$(BINARY_DIR)/spmv.cpu
PARTITION=$(BINARY_DIR)/partition
TARGETS=$(SPMV_MIC) $(SPMV_CPU) $(PARTITION)

all: $(TARGETS)

########################################
# SPMV CPU 
########################################
$(OBJECT_DIR)/%.o : CXXFLAGS += -xHOST
$(OBJECT_DIR)/%.o : %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(SPMV_CPU) : $(spmv_objects_cpu)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)


########################################
# SPMV MIC
########################################
$(OBJECT_DIR)/%.o.mic : CXXFLAGS += -mmic
$(OBJECT_DIR)/%.o.mic : %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@
$(SPMV_MIC) : CXXFLAGS += -mmic
$(SPMV_MIC) : $(spmv_objects_mic)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

########################################
# Partition 
########################################
$(PARTITION) : LDFLAGS += -lpatoh
$(PARTITION) : $(partition_objects)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS) 

check :
	@echo $(objects)

clean : 
	rm -f $(spmv_objects_cpu) 
	rm -f $(spmv_objects_mic) 
	rm -f $(partition_objects)
	rm -f $(SPMV_MIC)
	rm -f $(SPMV_CPU)
	rm -f $(PARTITION)

.PHONY : all clean check
