#!/bin/bash -x

sizes=(512 1024 2048 4096 8192 16384 32768 65536 131072)
nproc=(1 2 4 8 16 32 64)
for i in ${sizes[@]}
do
    for p in ${nproc[@]}
    do
        if [ $(($i/$p)) -gt 3200 ]; then
            continue
        fi
        ./generator band $i $p > band-$i-$p.mtx
    done
done

<<EOT
sizes=(512 1024 2048 4096 8192 16384)
for i in ${sizes[@]}
do
    ./generator unbalance $i > unbalance-$i.mtx
done

sizes=(1000 1100 1200 1300 1400 1500 1600 1700 1800 1900 2000)
for i in ${sizes[@]}
do
    ./generator dense $i > dense-$i.mtx
done
EOT
