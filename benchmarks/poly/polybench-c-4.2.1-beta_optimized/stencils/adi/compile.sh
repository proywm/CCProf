export CFLAGS="-g -O0 -fopenmp"
make
mv adi adi_O0
export CFLAGS="-g -O3 -fopenmp"
make
mv adi adi_O3

