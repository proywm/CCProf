BENCHMARK_RELATIVE_LOCATION=$CCPROFDIR/../benchmarks/himenoBMT/himenoBMT_optimized
BENCHMARK_BINARY=$BENCHMARK_RELATIVE_LOCATION/himenoBMTxpa_O0
mkdir workspace
cd workspace

#setting sampling period of himenoBMT to 171(high frequency)
echo "171" > SampleRateThreshold
cp SampleRateThreshold $BENCHMARK_RELATIVE_LOCATION

WORKSPACE=$PWD


echo $BENCHMARK_RELATIVE_LOCATION
cd $BENCHMARK_RELATIVE_LOCATION

#compile it
sh compile.sh

#second parameter specify the interation count
OMP_NUM_THREADS=1 sh $CCPROFDIR/runCCprof.sh $BENCHMARK_BINARY S 20

mv Data* $WORKSPACE

cd $WORKSPACE

hpcstruct $BENCHMARK_BINARY

cp $CCPROFDIR/../scripts/* .
mkdir modelInput

#using model trained with sampling period of 171
cp $CCPROFDIR/../modelInput/freq_171.csv modelInput/model_training_input.csv

#preprocessing generated data
sh convertscript.sh

#print file and line of loop from ip address
while read p; do
  echo "$p:"
  addr2line -e $BENCHMARK_BINARY $p
done <$WORKSPACE/LoopAddresses


#performing CCProf analysis
python ccProfAnalysis.py
