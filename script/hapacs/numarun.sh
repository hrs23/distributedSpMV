#!/bin/sh
# for block 
CPUNODE=`expr $PMI_RANK % 4`
numactl --cpunodebind=$CPUNODE --localalloc $@

