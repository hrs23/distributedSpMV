#/bin/bash -x
if [ -z "$SPMV_DIR" ]; then
    echo "Error: set \$SPMV_DIR"
    exit 
fi
#if [ $# -ne 1 ]; then
    #echo "Usage: $0 <npart>"
    #exit
#fi
ncore=4

matrices=`ls $SPMV_DIR/matrix/*.mtx | xargs -i basename {} | tr ' ' '\n'`
for ((npart=1; npart <= 64; npart *= 2))
do
    echo "npart = $npart"
    ls $SPMV_DIR/matrix/*.mtx | xargs -i basename {} | tr ' ' '\n' | xargs -P $ncore -I@ \
        $SPMV_DIR/bin/partition $SPMV_DIR/matrix/@ $npart $SPMV_DIR/partition/
done
