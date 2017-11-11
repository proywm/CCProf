BENCHMARK_RELATIVE_LOCATION=$CCPROFDIR/../benchmarks/dftc
BENCHMARK_BINARY=$BENCHMARK_RELATIVE_LOCATION/_results/intel_lp64_sequential_libem64t/basic_dp_complex_dft_2d_4096_pad.out
mkdir workspace
cd workspace

echo "1212" > SampleRateThreshold
cp SampleRateThreshold $BENCHMARK_RELATIVE_LOCATION

WORKSPACE=$PWD


echo $BENCHMARK_RELATIVE_LOCATION
cd $BENCHMARK_RELATIVE_LOCATION

#compile it
sh compile.sh

OMP_NUM_THREADS=1 sh $CCPROFDIR/runCCprof.sh $BENCHMARK_BINARY

mv Data* $WORKSPACE

cd $WORKSPACE

hpcstruct $BENCHMARK_BINARY

cp $CCPROFDIR/../scripts/* .
mkdir modelInput
cp $CCPROFDIR/../modelInput/freq_1212.csv modelInput/model_training_input.csv

#preprocessing generated data
sh convertscript.sh

#print file and line of loop from ip address
while read p; do
  echo "$p:"
  addr2line -e $BENCHMARK_BINARY $p
done <$WORKSPACE/LoopAddresses


#performing CCProf analysis
python ccProfAnalysis.py
