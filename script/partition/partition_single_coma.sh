#!/bin/bash
#SBATCH -J "SPMV-PARTITIONING"
#SBATCH -p mixed
#SBATCH -N 1
#SBATCH -n 1
#SBATCH --ntasks-per-node=1
#SBATCH --cpus-per-task=10
#SBATCH -t 10:00:00
#SBATCH -o stdout
#SBATCH -e stderr
#SBATCH -m block

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
matrices=`ls $SPMV_DIR/matrix/*.mtx | xargs -i basename {}`
for matrix in $matrices
do
    for ((npart=1; npart <= 64; npart *= 2))
    do
        echo "partitioning $matrix in $npart parts with $PARTITION_METHOD ... "
        $SPMV_DIR/bin/partition $SPMV_DIR/matrix/$matrix $PARTITION_METHOD $npart $SPMV_DIR/partition/$PARTITION_METHOD/
        echo "done"
    done
done

PARTITION_METHOD=hypergraph
matrices=`ls $SPMV_DIR/matrix/*.mtx | xargs -i basename {}`
for matrix in $matrices
do
    for ((npart=1; npart <= 64; npart *= 2))
    do
        echo "partitioning $matrix in $npart parts with $PARTITION_METHOD ... "
        $SPMV_DIR/bin/partition $SPMV_DIR/matrix/$matrix $PARTITION_METHOD $npart $SPMV_DIR/partition/$PARTITION_METHOD/
        echo "done"
    done
done
