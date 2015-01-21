#/bin/bash -x
if [ -z "$SPMV_DIR" ]; then
    echo "Error: set \$SPMV_DIR"
    exit 
fi
#if [ $# -ne 1 ]; then
    #echo "Usage: $0 <npart>"
    #exit
#fi
#npart=$1

matrices=`ls $SPMV_DIR/matrix/*.mtx | xargs -i basename {}`
for ((npart=1; npart <= 64; npart *= 2))
do
for matrix in $matrices
do
    echo -n "partitioning $matrix ... "
    $SPMV_DIR/bin/partition $SPMV_DIR/matrix/$matrix $npart $SPMV_DIR/partition/
    echo "done"
done
done
