#!/bin/bash
#SBATCH -J "Benchmark"
#SBATCH -p mixed
#SBATCH -N 16
#SBATCH --ntasks-per-node=2
#SBATCH -t 00:05:00
#SBATCH -o slurm/%j.out
#SBATCH -e slurm/%j.err
module load intelmpi/5.0.1 
cd $SLURM_SUBMIT_DIR
mpirun "./barrier 10000"
