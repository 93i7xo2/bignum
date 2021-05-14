set ylabel 'time (ms)'
set xlabel 'fib(n)'
set terminal png font " Times_New_Roman,12 "
set key left
set output "output.png"
set grid
set logscale y 10
plot "test_bn.dat" u 1:($2*1000) w lines title "bignum", \
     "test_bn_cpython.dat" u 1:($2*1000) w lines title "cpython"