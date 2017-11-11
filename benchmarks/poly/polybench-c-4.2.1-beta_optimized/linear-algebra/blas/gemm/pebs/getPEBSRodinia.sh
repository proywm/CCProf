#!/usr/bin/bash
BenchmarkOutDir=/home/proy/cTool/benchmarks_results/rodinia/cmiss_test_8
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
export OMP_NUM_THREADS=16
export OMP_STACKSIZE=16M
export OMP_PLACES="{0},{32},{1},{33},{2},{34},{3},{35},{4},{36},{5},{37},{6},{38},{7},{39}"

isSMT=SMT
#sh getPEBSScript.sh benchmarkListRodiniaTest benchmarkRunCommandRodinia16threadsTest $BenchmarkOutDir/SMT_NotDynamic $LIBMONITORDIR $ROOTDIR $isSMT

export OMP_PROC_BIND=true
export OMP_DISPLAY_ENV=TRUE
export OMP_DYNAMIC=false
export OMP_NUM_THREADS=8
export OMP_STACKSIZE=16M
export OMP_PLACES="{0},{1},{2},{3},{4},{5},{6},{7}"

#sh $MemToolDir/gompBindNONSMT.sh
mkdir -p $BenchmarkOutDir/NONSMT_NotDynamic
echo "Binding to NON SMT"
isSMT=NONSMT
sh getPEBSScript.sh benchmarkListRodiniaTest benchmarkRunCommandRodinia8threadsTest $BenchmarkOutDir/NONSMT_NotDynamic $LIBMONITORDIR $ROOTDIR $isSMT

