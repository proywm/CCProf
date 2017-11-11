ROOTDIR=$PWD 
PAPIDIR=$ROOTDIR/PAPI/PAPI
echo "Building PAPI--->"
echo "PAPI Directory:" 
echo $PAPIDIR
cd $PAPIDIR/src
make clean
./configure --prefix=$PAPIDIR/install CC=gcc
make -j 30
make install
echo "PAPI Build Complete"
cp ../install/lib*/libpapi.* libpfm4/lib/
cd $ROOTDIR
