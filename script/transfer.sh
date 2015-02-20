if [ "${SPMV_DIR-undefined}" = "undefined" ]; then
    echo 'Error: set \$SPMV_DIR'
    exit 
fi

tasks=""
for f in `ls $SPMV_DIR/partition/simple/ | xargs -i basename {}`
do
    PARTITION_DIR=$SPMV_DIR/partition/
    tasks+="scp $PARTITION_DIR/hypergraph/$f mhrs@coma:/work/NUMLIB2/mhrs/distributedSpMV/partition-tmp/hypergraph/\n"
    tasks+="scp $PARTITION_DIR/simple/$f mhrs@coma:/work/NUMLIB2/mhrs/distributedSpMV/partition-tmp/simple/\n"
    tasks+="scp $PARTITION_DIR/hypergraph/$f mhrs@hapacs:/work/NUMLIB/mhrs/distributedSpMV/partition-tmp/hypergraph/\n"
    tasks+="scp $PARTITION_DIR/simple/$f mhrs@hapacs:/work/NUMLIB/mhrs/distributedSpMV/partition-tmp/simple/\n"
done

echo -e $tasks | xargs -P 4 -I@ -t sh -c "eval @"
