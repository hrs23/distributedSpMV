#!/bin/bash
if [ $# -ne 2 ]; then
    echo "Usage: $0 <matrix> <partition method (simple or hypergraph)"
    exit
fi
matrix=$1
PARTITION_METHOD=$2
SPMV_DIR=/work/NUMLIB2/mhrs/distributedSpMV
PARTITION_DIR=$SPMV_DIR/partition/$PARTITION_METHOD
#echo `hostname` : $matrix - $PMI_SIZE - $PMI_RANK
cp "$PARTITION_DIR/$matrix-$PMI_SIZE-$PMI_RANK.part" /mic-work/$USER/
