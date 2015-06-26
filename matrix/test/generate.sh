#!/bin/bash -x

sizes=(512 1024 2048 4096 8192 16384 32768 65536 )
for i in ${sizes[@]}
do
    ./generator band $i > band-$i.mtx
done

sizes=(512 1024 2048 4096 8192 16384)
for i in ${sizes[@]}
do
    ./generator unbalance $i > unbalance-$i.mtx
done

sizes=(1024 2048 4096 5000 6000 7000 8192)
for i in ${sizes[@]}
do
    ./generator dense $i > dense-$i.mtx
done
