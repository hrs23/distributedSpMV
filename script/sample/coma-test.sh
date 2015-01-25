#!/bin/bash
#SBATCH -J "SPMV-OFFLOAD"
#SBATCH -p mixed
#SBATCH -N 1
#SBATCH -n 2
#SBATCH --ntasks-per-node=2
#SBATCH --cpus-per-task=10
#SBATCH -t 00:05:00
#SBATCH -o stdout
#SBATCH -e stderr
#SBATCH -m block
SPMV_DIR=/work/NUMLIB2/mhrs/distributedSpMV/
MATRIX_DIR=$SPMV_DIR/matrix/
PARTITION_DIR=$SPMV_DIR/partition
LOG=$SPMV_DIR/log/cpu_`date +%y_%m_%d`
SPMV=$SPMV_DIR/bin/spmv

cd $SPMV_DIR

module load intel/15.0.0 intelmpi/5.0.1 mkl/11.1.2
cmake .
make
export OMP_NUM_THREADS=10
export KMP_AFFINITY=compact
matrices=`ls ${MATRIX_DIR}/*.mtx | xargs -i basename {}`
for matrix in $matrices
do
    mpirun -np 2 $SPMV $PARTITION_DIR/$matrix $MATRIX_DIR/$matrix > $LOG
done
