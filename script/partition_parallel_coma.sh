#!/bin/bash
#SBATCH -J "SPMV-PARTITIONING"
#SBATCH -p mixed
#SBATCH -N 1
#SBATCH -n 1
#SBATCH --ntasks-per-node=1
#SBATCH --cpus-per-task=10
#SBATCH -t 05:00:00
#SBATCH -o stdout
#SBATCH -e stderr
#SBATCH -m block

CORE=20
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
