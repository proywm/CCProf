ROOTDIR=$PWD 
PAPIDIR=$ROOTDIR/PAPI/PAPI
LIBMONITORDIR=$ROOTDIR/libmonitor/libmonitor
echo "libmonitor Directory:"
echo $LIBMONITORDIR
cd $LIBMONITORDIR
make clean
rm -rf $LIBMONITORDIR/installDir/*
echo "PAPI Directory:" 
echo $PAPIDIR
rm -rf $PAPIDIR/install/*
cd $PAPIDIR/src/examples/
make clean
echo "cleaned"
cd $ROOTDIR
