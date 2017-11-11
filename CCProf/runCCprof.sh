#ROOTDIR=$PWD
#ROOTDIR=/home/proy/CCProf/MemTool
ROOTDIR=$CCPROFDIR
#sh $ROOTDIR/gompBind.sh
LIBMONITORDIR=$ROOTDIR/libmonitor/libmonitor
PAPIDIR=$ROOTDIR/PAPI/PAPI
foo=$(echo $1| sed -r 's/^.{2}//')
echo ${foo##*/} > memToolInfoForClient
#echo "100051" > SampleRateThreshold
#echo "200051" > SampleRateThreshold
#echo "10051" > SampleRateThreshold
#echo "1051" > SampleRateThreshold
#echo "953" > SampleRateThreshold
#echo "1212" > SampleRateThreshold
#echo "131" > SampleRateThreshold
#echo "809" > SampleRateThreshold
#echo "701" > SampleRateThreshold
#echo "521" > SampleRateThreshold
#echo "311" > SampleRateThreshold
#echo "300" > SampleRateThreshold
#echo "97" > SampleRateThreshold
#echo "171" > SampleRateThreshold
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$PAPIDIR/src/libpfm4/lib
#export LD_LIBRARY_PATH=$PAPIDIR/src/libpfm4/lib
$LIBMONITORDIR/installDir/bin/monitor-run -i $ROOTDIR/libPapiMonitor.so $*
