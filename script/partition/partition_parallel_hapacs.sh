#!/bin/bash
#PBS -S /bin/bash
#PBS -N PARTITIONING
#PBS -A NUMLIB
#PBS -q comq
#PBS -l select=1
#PBS -l walltime=03:00:00
#PBS -o pbs/
#PBS -e pbs/

module load intel/14.0.4 intelmpi/5.0.0 mkl/11.1.3
SPMV_DIR=/work/NUMLIB/mhrs/distributedSpMV/
MATRIX_DIR=$SPMV_DIR/matrix/
PARTITION_METHOD=simple
make bin/partition

CORE=16
tasks=""
matrices=`ls $SPMV_DIR/matrix/*.mtx | xargs -i basename {}`
for matrix in $matrices
do
    for ((npart=1; npart <= 64; npart *= 2))
    do
        tasks+="$SPMV_DIR/bin/partition $SPMV_DIR/matrix/$matrix simple $npart $SPMV_DIR/partition/$PARTITION_METHOD/\n"
        tasks+="$SPMV_DIR/bin/partition $SPMV_DIR/matrix/$matrix hypergraph $npart $SPMV_DIR/partition/$PARTITION_METHOD/\n"
    done
done
echo -e $tasks | xargs -P $CORE -I@ -t sh -c "eval @"
