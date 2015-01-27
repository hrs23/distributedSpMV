#/bin/bash
if [ $# -ne 1 ]; then
    echo "Usage: $0 <matrix>"
    exit 1
fi

./bin/partition matrix/$1 2 partition/
srun -p FATE  mpirun -np 2 bin/spmv partition/$1 matrix/$1


