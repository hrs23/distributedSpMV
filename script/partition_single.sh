#/bin/bash -x
if [ -z "$SPMV_DIR" ]; then
    echo "Error: set \$SPMV_DIR"
    exit 
fi
if [ $# -ne 1 ]; then
    echo "Usage: $0 <Partition method ('hypergraph' or 'simple')>"
    exit
fi
PARTITION_METHOD=$1
matrices=`ls $SPMV_DIR/matrix/*.mtx | xargs -i basename {}`
for matrix in $matrices
do
    for ((npart=1; npart <= 64; npart *= 2))
    do
        echo "partitioning $matrix in $npart parts with $PARTITION_METHOD ... "
        $SPMV_DIR/bin/partition $SPMV_DIR/matrix/$matrix $PARTITION_METHOD $npart $SPMV_DIR/partition/$PARTITION_METHOD/
        echo "done"
    done
done
