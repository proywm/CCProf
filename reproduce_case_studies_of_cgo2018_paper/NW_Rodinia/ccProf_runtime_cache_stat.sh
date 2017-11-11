#!/bin/bash
BENCHMARK_RELATIVE_LOCATION=$CCPROFDIR/../benchmarks/rodinia_3.1/openmp/nw
BENCHMARK_BINARY=$BENCHMARK_RELATIVE_LOCATION/needle_O0
BENCHMARK_BINARY_O3=$BENCHMARK_RELATIVE_LOCATION/needle_O3
mkdir workspace
cd workspace

WORKSPACE=$PWD
#######################################################################
echo "1212" > SampleRateThreshold
cp SampleRateThreshold $BENCHMARK_RELATIVE_LOCATION

echo $BENCHMARK_RELATIVE_LOCATION
cd $BENCHMARK_RELATIVE_LOCATION

sh compile.sh
starttime=$(date +%s.%N)
OMP_NUM_THREADS=1 sh $CCPROFDIR/runCCprof.sh $BENCHMARK_BINARY  16384 10 1
endtime=$(date +%s.%N)

runtime=$(python -c "print(${endtime} - ${starttime})")

cd $WORKSPACE
echo $runtime > runtime_ccProf

#########################################################################

cd $BENCHMARK_RELATIVE_LOCATION

starttime=$(date +%s.%N)
OMP_NUM_THREADS=1 $BENCHMARK_BINARY  16384 10 1
endtime=$(date +%s.%N)

runtime=$(python -c "print(${endtime} - ${starttime})")

cd $WORKSPACE
echo $runtime > runtime_without_ccProf

##########################################################################

cd $BENCHMARK_RELATIVE_LOCATION

starttime=$(date +%s.%N)
OMP_NUM_THREADS=1 $BENCHMARK_BINARY_O3  16384 10 1
endtime=$(date +%s.%N)

runtime=$(python -c "print(${endtime} - ${starttime})")

cd $WORKSPACE
echo $runtime > runtime_O3

##########################################################################

cd $BENCHMARK_RELATIVE_LOCATION

OMP_NUM_THREADS=1 sudo $CCPROFDIR/PAPI/PAPI/src/libpfm4/perf_examples/task -e L1-DCACHE-LOADS -e L1-DCACHE-LOAD-MISSES -e LLC_MISSES -e LLC_REFERENCES $BENCHMARK_BINARY_O3  16384 10 1> cacheOutScratch

mv cacheOutScratch $WORKSPACE/
cd $WORKSPACE
grep -Ri -C 0 "scaling, ena" cacheOutScratch > cacheOutScratch1
rm cacheStat_L1L_L1M_L2M_LLCM
awk '{if($2=="L1-DCACHE-LOADS")print $1}' cacheOutScratch1 >> cacheStat_L1L_L1M_L2M_LLCM
awk '{if($2=="L1-DCACHE-LOAD-MISSES")print $1}' cacheOutScratch1 >> cacheStat_L1L_L1M_L2M_LLCM
awk '{if($2=="LLC_REFERENCES")print $1}' cacheOutScratch1 >> cacheStat_L1L_L1M_L2M_LLCM
awk '{if($2=="LLC_MISSES")print $1}' cacheOutScratch1 >> cacheStat_L1L_L1M_L2M_LLCM

sed -i 's/,//g' cacheStat_L1L_L1M_L2M_LLCM

rm cacheOutScratch1



