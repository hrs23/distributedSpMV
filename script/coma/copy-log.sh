#!/bin/bash
if [ $# -ne 1 ]; then
    echo "Usage: $0 <log path> <destination>"
    exit
fi
LOG=$1
DST=$2
if [ $PMI_RANK -eq 0 ]; then
    cp "$LOG" "$DST"
fi
