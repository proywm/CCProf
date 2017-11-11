export CC=gcc
export CXX=g++
export FC=gfortran
export F77=gfortran

ROOTDIR=$PWD 
PAPIDIR=$ROOTDIR/PAPI/PAPI
LIBMONITORDIR=$ROOTDIR/libmonitor/libmonitor
echo "libmonitor Directory:"
echo $LIBMONITORDIR
cd $LIBMONITORDIR
make clean
./configure --prefix=$LIBMONITORDIR/installDir CC=gcc
make
make install
echo "Libmonitor installed *************"
echo "PAPI Directory:" 
echo $PAPIDIR
export LD_LIBRARY_PATH=$PAPIDIR/src/libpfm4/lib
cd $PAPIDIR/src/examples/
make clean
make
echo "Creating libmonitor.so file"
gcc -shared -g -o libPapiMonitor.so PAPI_overflow_libmonitor.o $PAPIDIR/src/libpfm4/lib/libpapi.so -L$PAPIDIR/src/libpfm4/lib/ -pthread -lrt
echo "Manually check whether libmonitor.so file created"
ldd libPapiMonitor.so
echo "Copy the file to Root Directior"
cp libPapiMonitor.so $ROOTDIR

cd $PAPIDIR/src/libpfm4/perf_examples
make
cd $ROOTDIR
