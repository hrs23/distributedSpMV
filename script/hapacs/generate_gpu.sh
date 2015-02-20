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

function max () {
    if [ $1 -lt $2 ]; then
        echo $1
    else 
        echo $2
    fi
}
for (( p=1; p <= 64; p*=2 ))
do

    RUN_SCRIPT=$SPMV_DIR/script/hapacs/gpu-$DISTRIBUTE_METHOD/run_p${p}.sh
    N=`echo ${p} | awk '{printf("%d",$1/4 + 0.9999999999)}'`
    mpiprocs=`max $p 4`
    ncpus=`expr $mpiprocs \* 4`
    echo "\
#!/bin/bash
#PBS -S /bin/bash
#PBS -N SPMV-G$D$p
#PBS -A NUMLIB
#PBS -q tcag
#PBS -l select=${N}:ncpus=$ncpus:mpiprocs=$mpiprocs:ompthreads=5
#PBS -l walltime=03:00:00
#PBS -l place=scatter
#PBS -o pbs/
#PBS -e pbs/

. /opt/Modules/default/init/bash
module load intelmpi/5.0.0 intel/14.0.4 cuda/6.5.14 cuda/samples_6.5.14 
cd $SPMV_DIR
export OMP_NUM_THREADS=5

MATRIX_DIR=${SPMV_DIR}/matrix/
PARTITION_DIR=${SPMV_DIR}/partition/$DISTRIBUTE_METHOD/
SPMV=${SPMV_DIR}/bin/spmv.gpu
LOG=${SPMV_DIR}/log/gpu-$DISTRIBUTE_METHOD-tca-p$p-\`date +%y-%m-%d\`.tsv
echo "" > \$LOG
make bin/spmv.gpu

matrices=\`ls \${MATRIX_DIR}/*.mtx | xargs -i basename {}\`
for matrix in \${matrices}
do
    mpirun -np ${p} -perhost ${mpiprocs} \$SPMV \$PARTITION_DIR/\$matrix >> \$LOG
done
    " > ${RUN_SCRIPT}
    chmod 700 ${RUN_SCRIPT}
done
