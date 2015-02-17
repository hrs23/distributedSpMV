#!/bin/bash
if [ -z '$SPMV_DIR' ]; then
    echo 'Error: set \$SPMV_DIR'
    exit 
fi
MAX_NPROC=64
DISTRIBUTE_METHOD=hypergraph
for (( p=1; p <= ${MAX_NPROC}; p*=2 ))
do

    RUN_SCRIPT=$SPMV_DIR/script/hapacs/cpu-$DISTRIBUTE_METHOD/run_p${p}.sh
    N=`echo ${p} | awk '{printf("%d",$1/2 + 0.5)}'`
    mpiprocs=`expr $p / $N`
    ncpus=`expr $mpiprocs \* 4`
    echo "\
#!/bin/bash
#PBS -S /bin/bash
#PBS -N SPMV-CH${p}
#PBS -A NUMLIB
#PBS -q comq
#PBS -l select=${N}:ncpus=$ncpus:mpiprocs=$mpiprocs
#PBS -l walltime=03:00:00
#PBS -l place=scatter
#PBS -o pbs/
#PBS -e pbs/

. /opt/Modules/default/init/bash
module load intel/14.0.4 intelmpi/5.0.0 mkl/11.1.3

cd $SPMV_DIR

MATRIX_DIR=${SPMV_DIR}/matrix/
PARTITION_DIR=${SPMV_DIR}/partition/$DISTRIBUTE_METHOD/
SPMV=${SPMV_DIR}/bin/spmv.cpu
LOG=${SPMV_DIR}/log/cpu-$DISTRIBUTE_METHOD-p$p-\`date +%y-%m-%d\`.tsv
echo "" > \$LOG
make bin/spmv.cpu

matrices=\`ls \${MATRIX_DIR}/*.mtx | xargs -i basename {}\`
for matrix in \${matrices}
do
    mpirun -np ${p} -perhost ${mpiprocs} \$SPMV \$PARTITION_DIR/\$matrix >> \$LOG
done
    " > ${RUN_SCRIPT}
    chmod 700 ${RUN_SCRIPT}
done
