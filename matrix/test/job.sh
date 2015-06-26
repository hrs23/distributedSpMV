#!/bin/bash
#SBATCH -J "GENMAT"
#SBATCH -p mixed
#SBATCH -N 1
#SBATCH -n 1
#SBATCH --ntasks-per-node=2
#SBATCH --cpus-per-task=10
#SBATCH -t 20:00:00
cd $SLURM_SUBMIT_DIR
./generate.sh
    
