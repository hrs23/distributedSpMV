#/bin/bash -x
set -u
if [ "${SPMV_DIR-undefined}" = "undefined" ]; then
    echo 'Error: set \$SPMV_DIR'
    exit 
fi

CORE=10
tasks=""
matrices=`ls $SPMV_DIR/matrix/tmp/*.mtx | xargs -i basename {}`
for matrix in $matrices
do
    for ((npart=1; npart <= 64; npart *= 2))
    do
        tasks+="$SPMV_DIR/bin/partition $SPMV_DIR/matrix/tmp/$matrix simple $npart $SPMV_DIR/partition/simple/\n"
        tasks+="$SPMV_DIR/bin/partition $SPMV_DIR/matrix/tmp/$matrix hypergraph $npart $SPMV_DIR/partition/hypergraph/\n"
    done
done
echo -e $tasks | xargs -P $CORE -I@ -t sh -c "eval @"
