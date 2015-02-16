#!/bin/bash
#PBS -S /bin/bash
#PBS -N PARTITIONING
#PBS -A NUMLIB
#PBS -q comq
#PBS -l select=1
#PBS -l walltime=03:00:00
#PBS -o pbs/
#PBS -e pbs/

if [ -z "$SPMV_DIR" ]; then
    echo "Error: set \$SPMV_DIR"
    exit 
fi


module load intel/15.0.0 intelmpi/5.0.1 mkl/11.1.2

#if [ $# -ne 1 ]; then
    #echo "Usage: $0 <Partition method ('hypergraph' or 'simple')>"
    #exit
#fi
PARTITION_METHOD=simple
for ((npart=1; npart <= 64; npart *= 2))
do
    echo "Partitioning to $npart with $PARTITION_METHOD ..."
    #ls $SPMV_DIR/matrix/*.mtx | xargs -i basename {} | tr ' ' '\n' | xargs -P $CORE -I@ 
    ls $SPMV_DIR/matrix/dense*.mtx | xargs -i basename {} | tr ' ' '\n' | xargs -P $CORE -I@ \
    $SPMV_DIR/bin/partition $SPMV_DIR/matrix/@ $PARTITION_METHOD $npart "$SPMV_DIR/partition/$PARTITION_METHOD/"
done

PARTITION_METHOD=hypergraph
for ((npart=1; npart <= 64; npart *= 2))
do
    echo "Partitioning to $npart with $PARTITION_METHOD ..."
    #ls $SPMV_DIR/matrix/*.mtx | xargs -i basename {} | tr ' ' '\n' | xargs -P $CORE -I@ 
    ls $SPMV_DIR/matrix/dense*.mtx | xargs -i basename {} | tr ' ' '\n' | xargs -P $CORE -I@ \
        $SPMV_DIR/bin/partition $SPMV_DIR/matrix/@ $PARTITION_METHOD $npart "$SPMV_DIR/partition/$PARTITION_METHOD/"
done
