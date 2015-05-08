#!/bin/bash
set -u

if [ $# -ne 1 ]; then
    echo "Usage: $0 <partition method>"
    exit 
fi
if [ "${SPMV_DIR-undefined}" = "undefined" ]; then
    echo 'Error: set \$SPMV_DIR'
    exit 
fi
if [ $1 != "hypergraph" -a $1 != "simple" ]; then
    echo "Error: partition method must be hypergraph or simple"
    exit
fi
DISTRIBUTE_METHOD=$1
D=`echo $1 | cut -b 1`
for (( p=1; p <= 64; p*=2 ))
do

    RUN_SCRIPT=$SPMV_DIR/script/coma/cpu-$DISTRIBUTE_METHOD/run_p${p}.sh
    N=`echo ${p} | awk '{printf("%d",$1/2 + 0.5)}'`
    echo "\
#!/bin/bash
#SBATCH -J \"SPMV-C$D$p\"
#SBATCH -p mixed
#SBATCH -N ${N}
#SBATCH -n ${p}
#SBATCH --ntasks-per-node=2
#SBATCH --cpus-per-task=10
#SBATCH -t 03:00:00
#SBATCH -o slurm/%j.out
#SBATCH -e slurm/%j.err
#SBATCH -m block:block
MATRIX_DIR=${SPMV_DIR}/matrix/
PARTITION_DIR=${SPMV_DIR}/partition/$DISTRIBUTE_METHOD/
SPMV=${SPMV_DIR}/bin/spmv.cpu
LOG=${SPMV_DIR}/log/cpu-$DISTRIBUTE_METHOD-p$p-\`date +%y-%m-%d\`.tsv
echo "" > \$LOG
cd $SPMV_DIR
module load intel/15.0.2 intelmpi/5.0.3 mkl/11.2.2
make bin/spmv.cpu
export OMP_NUM_THREADS=10
export KMP_AFFINITY=compact

matrices=\`ls \${MATRIX_DIR}/*.mtx | xargs -i basename {}\`
for matrix in \${matrices}
do
    #mpirun -np ${p} numactl --localalloc \$SPMV \$PARTITION_DIR/\$matrix >> \$LOG
    mpirun -np ${p} ${SPMV_DIR}/script/coma/numarun.sh \$SPMV \$PARTITION_DIR/\$matrix >> \$LOG
done
    " > ${RUN_SCRIPT}
    chmod 700 ${RUN_SCRIPT}
done
