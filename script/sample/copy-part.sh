#!/bin/bash
if [ $# -ne 1 ]; then
    echo "Usage: $0 <matrix>"
    exit
fi
SPMV_DIR=/work/NUMLIB2/mhrs/distributedSpMV
PARTITION_METHOD=simple
PARTITION_DIR=$SPMV_DIR/partition/$PARTITION_METHOD
matrix=$1
#echo `hostname` : $matrix - $PMI_SIZE - $PMI_RANK
cp "$PARTITION_DIR/$matrix-$PMI_SIZE-$PMI_RANK.part" /mic-work/$USER/
