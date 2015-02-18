#!/bin/bash
#SBATCH -J "SPMV-PARTITIONING"
#SBATCH -p mixed
#SBATCH -N 1
#SBATCH -n 1
#SBATCH --ntasks-per-node=1
#SBATCH --cpus-per-task=10
#SBATCH -t 30:00:00
#SBATCH -o stdout
#SBATCH -e stderr
#SBATCH -m block

if [ -z "$SPMV_DIR" ]; then
    echo "Error: set \$SPMV_DIR"
    exit 
fi

module load intel/15.0.0 intelmpi/5.0.1 mkl/11.1.2

CORE=20
tasks=""
matrices=`ls $SPMV_DIR/matrix/*.mtx | xargs -i basename {}`
for matrix in $matrices
do
    for ((npart=1; npart <= 64; npart *= 2))
    do
        tasks+="$SPMV_DIR/bin/partition $SPMV_DIR/matrix/tmp/$matrix simple $npart $SPMV_DIR/partition/simple/\n"
        tasks+="$SPMV_DIR/bin/partition $SPMV_DIR/matrix/tmp/$matrix hypergraph $npart $SPMV_DIR/partition/hypergraph/\n"
    done
done
echo -e $tasks | xargs -P $CORE -I@ -t sh -c "eval @"
