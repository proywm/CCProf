#!/usr/bin/bash
BenchmarkOutDir=/home/proy/cTool/benchmarks_results/rodinia/cmiss_test_1Thread_
MemToolDir=/home/proy/cTool/MemTool
mkdir -p  $BenchmarkOutDir
#without Dynamic
mkdir -p $BenchmarkOutDir/SMT_NotDynamic

ROOTDIR=/home/proy/cTool/MemTool
LIBMONITORDIR=$ROOTDIR/libmonitor/libmonitor
PAPIDIR=$ROOTDIR/PAPI/PAPI
export LD_LIBRARY_PATH=$PAPIDIR/src/libpfm4/lib
echo "LD_LIBRARY_PATH"
echo $LD_LIBRARY_PATH
echo "Binding to SMT"

export OMP_PROC_BIND=true
export OMP_DISPLAY_ENV=TRUE
export OMP_DYNAMIC=false
export OMP_NUM_THREADS=1
export OMP_STACKSIZE=16M
export OMP_PLACES="{0}"

#sh $MemToolDir/gompBindNONSMT.sh
mkdir -p $BenchmarkOutDir/NONSMT_NotDynamic
echo "Binding to NON SMT"
isSMT=NONSMT
sh getPEBSScript.sh benchmarkListRodinia1thread benchmarkRunCommandRodinia1thread $BenchmarkOutDir/NONSMT_NotDynamic $LIBMONITORDIR $ROOTDIR $isSMT

