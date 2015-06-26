#!/bin/bash
#SBATCH -J "TEST"
#SBATCH -p mixed
#SBATCH -N 1
##SBATCH -n 2
##SBATCH --ntasks-per-node=2
##SBATCH --cpus-per-task=10
#SBATCH -t 00:30:00
##SBATCH -o stdout
##SBATCH -e stderr

cd $SLURM_SUBMIT_DIR

module purge
module load intel/15.0.0 intelmpi/5.0.1 mkl/11.1.2

mpiicpc -static_mpi -mmic -o hello.mic hello.cpp

export MIC_PPN=1
export I_MPI_MIC=enable
export I_MPI_DEBUG=5
/opt/slurm/default/local/bin/mpirun-mic -v -m "./hello.mic" 
