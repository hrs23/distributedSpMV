#!/bin/bash
#SBATCH -J "SPMV-HOST-TEST"
#SBATCH -p mic
#SBATCH -N 10
#SBATCH -n 20
#SBATCH -t 00:05:00
#SBATCH -o stdout
#SBATCH -e stderr

module load intel/15.0.0 mkl/11.1.2 intelmpi/5.0.1
export MIC_PPN=1
export I_MPI_MIC=enable
mpirun $SPMV_DIR/script/hostname/myhostname
echo -------------------------
pdcp -w $SLURM_JOB_NODELIST -R ssh $SPMV_DIR/script/hostname/myhostname.mic /mic-work/$USER
/opt/slurm/default/local/bin/mpirun-mic2 -m /mic-work/$USER/myhostname.mic
