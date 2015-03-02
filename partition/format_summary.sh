#!/bin/bash
matrices=`ls ../matrix/*.mtx | xargs -i basename {}`
for matrix in $matrices
do
    for (( p=1; p<=64; p*=2)) 
    do  
        ./format_stat hypergraph/$matrix-$p.stat
    done
done
