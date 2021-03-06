#!/bin/bash
#PBS -S /bin/bash
#PBS -N PARTITIONING
#PBS -A NUMLIB
#PBS -q comq
#PBS -l select=1
#PBS -l walltime=20:00:00
#PBS -o pbs/
#PBS -e pbs/

set -u
if [ "${SPMV_DIR-undefined}" = "undefined" ]; then
    echo 'Error: set \$SPMV_DIR'
    exit 
fi
module load intel/14.0.4 intelmpi/5.0.0 mkl/11.1.3
SPMV_DIR=/work/NUMLIB/mhrs/distributedSpMV/
#MATRIX_DIR=$SPMV_DIR/matrix/tmp/
MATRIX_DIR=$SPMV_DIR/matrix/
cd $SPMV_DIR
make bin/partition

tasks=""
matrices=`ls $MATRIX_DIR/*.mtx | xargs -i basename {}`
for matrix in $matrices
do
    for ((npart=1; npart <= 64; npart *= 2))
    do
        tasks+="$SPMV_DIR/bin/partition $MATRIX_DIR/$matrix simple $npart $SPMV_DIR/partition/simple/\n"
        tasks+="$SPMV_DIR/bin/partition $MATRIX_DIR/$matrix hypergraph $npart $SPMV_DIR/partition/hypergraph/\n"
    done
done
echo -e $tasks | xargs -P 0 -I@ -t sh -c "eval @"
