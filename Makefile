SOURCE_DIR = src
BINARY_DIR = bin
OBJECT_DIR = obj
LIBRARY_DIR = lib
INCLUDE_DIR = include

#OPTION = -DPRINT_HOSTNAME -DPRINT_PERFORMANCE -DPRINT_REAL_PERFORMANCE -DPRINT_NUMABIND -DUSE_DENSE_INTERNAL_INDEX
OPTION = -DPRINT_HOSTNAME -DPRINT_PERFORMANCE 

CXX = mpiicpc
LDFLAGS = -L$(LIBRARY_DIR) -L$(OBJECT_DIR)
CXXFLAGS = -std=c++11 -ipo -Wall -O2 -fopenmp -I$(INCLUDE_DIR) $(OPTION)

vpath %.cpp $(SOURCE_DIR)
partition_sources = partition.cpp util.cpp
spmv_sources = main.cpp mpi_util.cpp spmv.cpp spmv_kernel.cpp util.cpp 

partition_objects = $(addprefix $(OBJECT_DIR)/, $(partition_sources:.cpp=.o))
spmv_objects_cpu = $(addprefix $(OBJECT_DIR)/, $(spmv_sources:.cpp=.o.cpu))
spmv_objects_mic = $(addprefix $(OBJECT_DIR)/, $(spmv_sources:.cpp=.o.mic))
spmv_objects_gpu = $(addprefix $(OBJECT_DIR)/, $(spmv_sources:.cpp=.o.gpu))

SPMV_CPU=$(BINARY_DIR)/spmv.cpu
SPMV_MIC=$(BINARY_DIR)/spmv.mic
SPMV_GPU=$(BINARY_DIR)/spmv.gpu
PARTITION=$(BINARY_DIR)/partition
TARGETS=$(SPMV_CPU) $(SPMV_MIC) $(SPMV_GPU) $(PARTITION)

all: $(TARGETS)

########################################
# SPMV CPU 
########################################
$(OBJECT_DIR)/%.o.cpu : CXXFLAGS += -xHOST -DCPU  
$(OBJECT_DIR)/%.o.cpu : %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(SPMV_CPU) : CXXFLAGS += -mkl
$(SPMV_CPU) : LDFLAGS += -lnuma
$(SPMV_CPU) : $(spmv_objects_cpu)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

########################################
# SPMV MIC
########################################
$(OBJECT_DIR)/%.o.mic : CXXFLAGS += -mmic -DMIC -DTHRESHOLD_SECOND=3.0
$(OBJECT_DIR)/%.o.mic : %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@
$(SPMV_MIC) : CXXFLAGS += -mmic -mkl
$(SPMV_MIC) : $(spmv_objects_mic)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

########################################
# SPMV GPU 
########################################
$(OBJECT_DIR)/%.o.gpu : CXXFLAGS += -xHOST -DGPU -DGPU_PER_NODE=4 -I/opt/CUDA/6.5.14/cudatoolkit/include -I/opt/CUDA/6.5.14/samples/common/inc
$(OBJECT_DIR)/%.o.gpu : %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

#$(SPMV_GPU) : CXXFLAGS += -L/opt/CUDA/6.5.14/cudatoolkit/lib64 -L/opt/CUDA/6.5.14/samples/common/lib -lcusparse -lcudart
$(SPMV_GPU) : CXXFLAGS += -L/opt/CUDA/6.5.14/cudatoolkit/lib64 -lcusparse -lcudart
$(SPMV_GPU) : $(spmv_objects_gpu)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)


########################################
# Partition 
########################################
$(PARTITION) : LDFLAGS += -lpatoh
$(OBJECT_DIR)/%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(PARTITION) : $(partition_objects)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS) 

check :
	@echo $(objects)

clean : 
	rm -f $(spmv_objects_cpu) 
	rm -f $(spmv_objects_mic) 
	rm -f $(spmv_objects_gpu) 
	rm -f $(partition_objects)
	rm -f $(SPMV_CPU)
	rm -f $(SPMV_MIC)
	rm -f $(SPMV_GPU)
	rm -f $(PARTITION)

.PHONY : all clean check
