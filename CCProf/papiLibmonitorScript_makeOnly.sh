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
echo "PAPI Directory:" 
echo $PAPIDIR
export LD_LIBRARY_PATH=$PAPIDIR/install/lib64
cd $PAPIDIR/src/examples/
make clean
make -j30
echo "Creating libmonitor.so file"
gcc -shared -g -o libPapiMonitor.so PAPI_overflow_libmonitor.o $PAPIDIR/src/libpfm4/lib/libpapi.so -L$PAPIDIR/src/libpfm4/lib/ -pthread -lrt
echo "Creating libmonitor.a file"
ar rcs libPapiMonitor.a PAPI_overflow_libmonitor.o $PAPIDIR/src/libpfm4/lib/libpapi.a

echo "Creating libmonitor_lat.so file"
gcc -shared -g -o libPapiMonitor_lat.so PAPI_overflow_libmonitor_LatencyEvent.o $PAPIDIR/src/libpfm4/lib/libpapi.so -L$PAPIDIR/src/libpfm4/lib/ -pthread -lrt
echo "Creating libmonitor_lat.a file"
ar rcs libPapiMonitor_lat.a PAPI_overflow_libmonitor_LatencyEvent.o $PAPIDIR/src/libpfm4/lib/libpapi.a

echo "Manually check whether libPapimonitor.so file created"
ldd libPapiMonitor.so
ldd libPapiMonitor_lat.so
echo "Copy the file to Root Directior"
cp libPapiMonitor.so $ROOTDIR
cp libPapiMonitor.a $ROOTDIR
cp libPapiMonitor_lat.so $ROOTDIR
cp libPapiMonitor_lat.a $ROOTDIR
cd $ROOTDIR
ls -l | grep libPapiMonitor*
