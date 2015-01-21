#!/bin/bash
#SBATCH -J "SPMV-OFFLOAD"
#SBATCH -p mixed
#SBATCH -N 2
#SBATCH -n 4
#SBATCH --ntasks-per-node=2
#SBATCH --cpus-per-task=10
#SBATCH -t 00:00:05
#SBATCH -o stdout
#SBATCH -e stderr
#SBATCH -m block
SPMV_DIR=/work/NUMLIB2/mhrs/distributedSpMV/
MATRIX_DIR=$SPMV_DIR/matrix/
PARTITION_DIR=$SPMV_DIR/partition
LOG=$SPMV_DIR/log/cpu_`date +%y_%m_%d`
SPMV=$SPMV_DIR/bin/spmv

cd $SPMV_DIR
module load intel/14.0.2 intelmpi/4.1.3 mkl/11.1.2
export OMP_NUM_THREADS=10
export KMP_AFFINITY=compact
matrices=`ls ${MATRIX_DIR}/*.mtx | xargs -i basename {}`
for matrix in $matrices
do
    mpirun -np 4 $SPMV $MATRIX_DIR/$matrix $PARTITION_DIR/$matrix  > $LOG
done


