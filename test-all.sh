#!/bin/sh
i=0
for backend in '' '-DGGT_SUPP_NATIVE=0' '-DGGT_SUPP_NATIVE=0 -DGGT_SUPP_TEAL=0'
do
    for thr in '' '-DGGT_SUPP_THREADS=1'
    do
        (
            gcc -O3 test.c $backend $thr libggt.a -o test > /dev/null 2>&1 &&
            ./test > /dev/null 2>&1 &&
            rm -f test
        ) || echo "Failed: $backend $thr"
        i=$((i+1))
    done
done
