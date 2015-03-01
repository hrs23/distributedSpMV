#!/bin/bash
matrices=`ls ../matrix/*.mtx | xargs -i basename {}`
for matrix in $matrices
do
    for (( p=1; p<=64; p*=2)) 
    do  
        ./format_stat simple/$matrix-$p.stat
    done
done
