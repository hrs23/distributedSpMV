#!/bin/bash
if [ -z '$SPMV_DIR' ]; then
    echo 'Error: set \$SPMV_DIR'
    exit 
fi
MAX_NPROC=64
DISTRIBUTE_METHOD=hypergraph
for (( p=1; p <= ${MAX_NPROC}; p*=2 ))
do

    RUN_SCRIPT=$SPMV_DIR/script/mic-$DISTRIBUTE_METHOD/run_p${p}.sh
    N=`echo ${p} | awk '{printf("%d",$1/2 + 0.5)}'`
    echo "\
#!/bin/bash
#SBATCH -J \"SPMV-MIC\"
#SBATCH -p mic
#SBATCH -N ${N}
#SBATCH -n ${p}
#SBATCH --ntasks-per-node=2
#SBATCH --cpus-per-task=10
#SBATCH -t 03:00:00
#SBATCH -o stdout
#SBATCH -e stderr
#SBATCH -m block
MATRIX_DIR=${SPMV_DIR}/matrix/
PARTITION_DIR=${SPMV_DIR}/partition/$DISTRIBUTE_METHOD/
SPMV=${SPMV_DIR}/bin/spmv
LOG=${SPMV_DIR}/log/mic-$DISTRIBUTE_METHOD-p$p-`date +%y-%m-%d`.tsv
echo "" > \$LOG
cd $SPMV_DIR
module load intel/15.0.0 intelmpi/5.0.1 mkl/11.1.2
#cmake .
#make
export MIC_PPN=1
export I_MPI_MIC=enable
export KMP_AFFINITY=compact
export MIC_ENV_PREFIX=MIC
export MIC_OMP_NUM_THREADS=240

matrices=\`ls \${MATRIX_DIR}/*.mtx | xargs -i basename {}\`
pdcp -w \$SLURM_JOB_NODELIST -R ssh $SPMV_DIR/bin/spmv.mic /mic-work/\$USER
for matrix in \${matrices}
do
    mpirun $SPMV_DIR/script/copy-part.sh \$matrix
    /opt/slurm/default/local/bin/mpirun-mic2 -m \"/mic-work/\$USER/spmv.mic /mic-work/\$USER/\$matrix\" >> \$LOG
    #mpirun $SPMV_DIR/script/copy-log.sh \$LOG $SPMV_DIR/log/
    #rpdcp -w \$SLURM_JOB_NODELIST -R ssh /mic-work/\$USER/\$LOG $SPMV_DIR/log/
done
    " > ${RUN_SCRIPT}
    chmod 700 ${RUN_SCRIPT}
done
