#!/bin/bash
set -u
if [ "${SPMV_DIR-undefined}" = "undefined" ]; then
    echo 'Error: set \$SPMV_DIR'
    exit 
fi

$SPMV_DIR/script/coma/generate_mic.sh simple
$SPMV_DIR/script/coma/generate_mic.sh hypergraph
$SPMV_DIR/script/coma/generate_cpu.sh simple
$SPMV_DIR/script/coma/generate_cpu.sh hypergraph
