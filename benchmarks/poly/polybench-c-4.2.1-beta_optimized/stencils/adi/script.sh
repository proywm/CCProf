make clean

gcc -o adi adi.c -g -O2 -DPOLYBENCH_TIME -I. -I../../utilities ../../utilities/polybench.c

./adi

make clean

gcc -o adi adi.c -g -O2 -DPOLYBENCH_PAPI -I/home/proy/papi-5.4.1/install/include -I. -I../../utilities ../../utilities/polybench.c -L/home/proy/papi-5.4.1/install/lib64 -lpapi
./adi
