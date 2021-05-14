#!/bin/bash
rm -f *.dat
for i in {1..10000..10}; do
    echo -n "$i " >>./test_bn.dat
    ./test_bn $i 1>>./test_bn.dat
    echo -n "$i " >>./test_bn_cpython.dat
    ./test_bn_cpython $i 1>>./test_bn_cpython.dat
done
gnuplot ./plot.gp