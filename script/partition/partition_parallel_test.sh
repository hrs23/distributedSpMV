#/bin/bash
set -u
if [ "${SPMV_DIR-undefined}" = "undefined" ]; then
    echo 'Error: set \$SPMV_DIR'
    exit 
fi
MATRIX_DIR=$SPMV_DIR/matrix/test/
cd $SPMV_DIR
make bin/partition
tasks=""
matrices=`ls $MATRIX_DIR/dense*.mtx | xargs -i basename {}`
for matrix in $matrices
do
    for ((npart=1; npart <= 64; npart *= 2))
    do
        tasks+="$SPMV_DIR/bin/partition $MATRIX_DIR/$matrix simple $npart $SPMV_DIR/partition/test/simple/\n"
        tasks+="$SPMV_DIR/bin/partition $MATRIX_DIR/$matrix hypergraph $npart $SPMV_DIR/partition/test/hypergraph/\n"
    done
done
echo -e $tasks | xargs -P 4 -I@ -t sh -c "eval @"
