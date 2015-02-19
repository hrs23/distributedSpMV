#!/bin/bash
set -u
if [ "${SPMV_DIR-undefined}" = "undefined" ]; then
    echo 'Error: set \$SPMV_DIR'
    exit 
fi

$SPMV_DIR/script/hapacs/generate_gpu.sh simple
$SPMV_DIR/script/hapacs/generate_gpu.sh hypergraph
$SPMV_DIR/script/hapacs/generate_cpu.sh simple
$SPMV_DIR/script/hapacs/generate_cpu.sh hypergraph
