#!/bin/sh
# for block 
CPUNODE=`expr $PMI_RANK / 2 % 2`
numactl --cpunodebind=$CPUNODE --localalloc $@

