#/bin/bash
SPMV_DIR=~/distributedSpMV/
matrix_files=`ls ${SPMV_DIR}/matrix/*.mtx | xargs -i basename {}`

nproc=4
export OMP_NUM_THREADS=8
for matrix in ${matrix_files}
do
    echo "-----------"
    echo "${matrix}"
    echo "-----------"
    ${SPMV_DIR}/bin/partition ${SPMV_DIR}/matrix/${matrix} ${nproc} partition/
    salloc -p FATE -N 4 --ntasks-per-node=1 mpirun ${SPMV_DIR}/bin/spmv ${SPMV_DIR}/partition/${matrix} ${SPMV_DIR}matrix/${matrix}
done

