#!/bin/bash
if [ -z '$SPMV_DIR' ]; then
    echo 'Error: set \$SPMV_DIR'
    exit 
fi
MAX_NPROC=64
DISTRIBUTE_METHOD=hypergraph
for (( p=1; p <= ${MAX_NPROC}; p*=2 ))
do

    RUN_SCRIPT=$SPMV_DIR/script/cpu-$DISTRIBUTE_METHOD/run_p${p}.sh
    N=`echo ${p} | awk '{printf("%d",$1/2 + 0.5)}'`
    echo "\
#!/bin/bash
#SBATCH -J \"SPMV-CH${p}\"
#SBATCH -p mixed
#SBATCH -N ${N}
#SBATCH -n ${p}
#SBATCH --ntasks-per-node=2
#SBATCH --cpus-per-task=10
#SBATCH -t 03:00:00
#SBATCH -o slurm/%j.out
#SBATCH -e slurm/%j.err
#SBATCH -m block
MATRIX_DIR=${SPMV_DIR}/matrix/
PARTITION_DIR=${SPMV_DIR}/partition/$DISTRIBUTE_METHOD/
SPMV=${SPMV_DIR}/bin/spmv.cpu
LOG=${SPMV_DIR}/log/cpu-$DISTRIBUTE_METHOD-p$p-\`date +%y-%m-%d\`.tsv
echo "" > \$LOG
cd $SPMV_DIR
module load intel/15.0.0 intelmpi/5.0.1 mkl/11.1.2
make
export OMP_NUM_THREADS=10
export KMP_AFFINITY=compact

matrices=\`ls \${MATRIX_DIR}/*.mtx | xargs -i basename {}\`
for matrix in \${matrices}
do
    mpirun -np ${p} \$SPMV \$PARTITION_DIR/\$matrix >> \$LOG
done
    " > ${RUN_SCRIPT}
    chmod 700 ${RUN_SCRIPT}
done
