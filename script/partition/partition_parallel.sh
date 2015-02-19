#/bin/bash -x
set -u
if [ "${SPMV_DIR-undefined}" = "undefined" ]; then
    echo 'Error: set \$SPMV_DIR'
    exit 
fi
MATRIX_DIR=$SPMV_DIR/matrix/
cd $SPMV_DIR
make bin/partition
tasks=""
matrices=`ls $MATRIX_DIR/*.mtx | xargs -i basename {}`
for matrix in $matrices
do
    for ((npart=1; npart <= 64; npart *= 2))
    do
        tasks+="$SPMV_DIR/bin/partition $MATRIX_DIR/$matrix simple $npart $SPMV_DIR/partition/simple/\n"
        tasks+="$SPMV_DIR/bin/partition $MATRIX_DIR/$matrix hypergraph $npart $SPMV_DIR/partition/hypergraph/\n"
    done
done
echo -e $tasks | xargs -P 4 -I@ -t sh -c "eval @"
