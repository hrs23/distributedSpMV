#!/bin/bash
#SBATCH -J "SPMV-NATIVE-TEST"
#SBATCH -p mic
#SBATCH -N 2
#SBATCH --ntasks-per-node=2
#SBATCH -t 01:00:00
#SBATCH -o stdout
#SBATCH -e stderr

nproc=4
PARTITION_METHOD=simple
SPMV_DIR=/work/NUMLIB2/mhrs/distributedSpMV/
MATRIX_DIR=$SPMV_DIR/matrix/
PARTITION_DIR=$SPMV_DIR/partition
LOG=mic_`date +%y_%m_%d`.tsv
SPMV=$SPMV_DIR/bin/spmv

export MIC_PPN=1
export I_MPI_MIC=enable
cd $SPMV_DIR

module load intel/15.0.0 mkl/11.1.2 intelmpi/5.0.1
#cmake .
#make
export OMP_NUM_THREADS=10
export KMP_AFFINITY=compact
#export MPIEXEC_FLAGS_MIC="-wdir /mic-work/$USER"

pdcp -w $SLURM_JOB_NODELIST -R ssh $SPMV_DIR/bin/spmv /mic-work/$USER
matrices=`ls ${MATRIX_DIR}/*.mtx | xargs -i basename {}`

for matrix in $matrices
do
    mpirun $SPMV_DIR/script/sample/copy-part.sh $matrix
    #HOSTS=`scontrol show hostname $SLURM_NODELIST`
    #awk -v dir="$PARTITION_DIR/$PARTITION_METHOD/" -v m="$matrix" -v np="$nproc" '{print dir m "-" np "-" NR-1 ".part" " " $1 ":/mic-work/$USER/"}' | tr '\n' ' ' | xargs -t -n 2 -P 2 scp -o "StrictHostKeyChecking no"
    #pdcp -w $SLURM_JOB_NODELIST -R ssh $PARTITION_DIR/hypergraph/$matrix-[0-$nproc]-$nproc.part /mic-work/$USER/
    #/opt/slurm/default/local/bin/mpirun-mic2 -m /mic-work/$USER/spmv >> $LOG
    /opt/slurm/default/local/bin/mpirun-mic2 -m "/mic-work/$USER/spmv /mic-work/$USER/$matrix"
done
#rpdcp -w $SLURM_JOB_NODELIST -R ssh /mic-work/$USER/$LOG $SPMV_DIR/log/
