BENCHMARK_RELATIVE_LOCATION=$CCPROFDIR/../benchmarks/rodinia_3.1/openmp/nw_padded/
BENCHMARK_BINARY=$BENCHMARK_RELATIVE_LOCATION/needle_O0
mkdir workspace
cd workspace

echo "1212" > SampleRateThreshold
cp SampleRateThreshold $BENCHMARK_RELATIVE_LOCATION

WORKSPACE=$PWD


echo $BENCHMARK_RELATIVE_LOCATION
cd $BENCHMARK_RELATIVE_LOCATION

#compile it
sh compile.sh

OMP_NUM_THREADS=1 sh $CCPROFDIR/runCCprof.sh $BENCHMARK_BINARY 16384 10 1

mv Data* $WORKSPACE

cd $WORKSPACE

hpcstruct $BENCHMARK_BINARY

cp $CCPROFDIR/../scripts/* .
mkdir modelInput
cp $CCPROFDIR/../modelInput/freq_1212.csv modelInput/model_training_input.csv

#preprocessing generated data
sh convertscript.sh
#performing CCProf analysis
python ccProfCDFdata.py

