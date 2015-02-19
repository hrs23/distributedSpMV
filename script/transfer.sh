if [ "${SPMV_DIR-undefined}" = "undefined" ]; then
    echo 'Error: set \$SPMV_DIR'
    exit 
fi

tasks=""
for p in `ls $SPMV_DIR/partition/simple/`
do
    tasks+="scp $PARTITION_DIR/partition/hypergraph/* mhrs@coma:/work/NUMLIB2/mhrs/distributedSpMV/partition-tmp/hypergraph/"
    tasks+="scp $PARTITION_DIR/partition/simple/* mhrs@coma:/work/NUMLIB2/mhrs/distributedSpMV/partition-tmp/simple/"
    tasks+="scp $PARTITION_DIR/partition/hypergraph/* mhrs@hapacs:/work/NUMLIB/mhrs/distributedSpMV/partition-tmp/hypergraph/"
    tasks+="scp $PARTITION_DIR/partition/simple/* mhrs@hapacs:/work/NUMLIB/mhrs/distributedSpMV/partition-tmp/simple/"
done

echo $tasks
#echo -e $tasks | xargs -P 4 -I@ -t sh -c "eval @"
