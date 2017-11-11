BENCHMARK_RELATIVE_LOCATION=$CCPROFDIR/../benchmarks/tinyDNN/tiny-dnn-0.1.1
BENCHMARK_BINARY=$BENCHMARK_RELATIVE_LOCATION/build/example_cifar_train
mkdir workspace
cd workspace

echo "1212" > SampleRateThreshold
cp SampleRateThreshold $BENCHMARK_RELATIVE_LOCATION

WORKSPACE=$PWD


echo $BENCHMARK_RELATIVE_LOCATION
cd $BENCHMARK_RELATIVE_LOCATION

#compile it
sh compile.sh

# please download and set the cifar data 
wget http://www.cs.toronto.edu/~kriz/cifar-10-binary.tar.gz
tar -xvf cifar-10-binary.tar.gz

#ctrl+c to exit in the middle.. rerun this script commenting following line
OMP_NUM_THREADS=1 sh $CCPROFDIR/runCCprof.sh $BENCHMARK_BINARY $BENCHMARK_RELATIVE_LOCATION/cifardata/cifar-10-batches-bin 0.01

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

