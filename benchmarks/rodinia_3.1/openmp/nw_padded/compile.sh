export CC_FLAGS="-g -O0 -fopenmp"
make
mv needle needle_O0
export CC_FLAGS="-g -O3 -fopenmp"
make
mv needle needle_O3

