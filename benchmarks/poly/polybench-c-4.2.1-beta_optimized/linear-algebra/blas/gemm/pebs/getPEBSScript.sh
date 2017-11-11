#!/usr/bin/bash
benchmarkList_filename="$1"
benchmarkCommand_filename="$2"
benchmarkCommand_outputDir="$3"
LIBMONITORDIR="$4"
ROOTDIR="$5"
paste $benchmarkList_filename $benchmarkCommand_filename | while read -r benchmarkName benchmark_Command
do
    #name=$line
    mkdir -p $benchmarkCommand_outputDir/$benchmarkName/
    echo $benchmarkName > memToolInfoForClient
    echo $LIBMONITORDIR
    echo "Running Benchmark - $benchmarkName -> run Command: $LIBMONITORDIR/installDir/bin/monitor-run -i $ROOTDIR/libPapiMonitor.so  $benchmark_Command"
    echo "creating " > $benchmarkCommand_outputDir/$benchmarkName/output.txt
    PAPIDIR=/home/proy/cTool/MemTool/PAPI/PAPI
    export LD_LIBRARY_PATH=$PAPIDIR/src/libpfm4/lib
    #echo "export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$PAPIDIR/src/libpfm4/lib"
    (time LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/home/proy/cTool/MemTool/PAPI/PAPI/src/libpfm4/lib $LIBMONITORDIR/installDir/bin/monitor-run -i $ROOTDIR/libPapiMonitor.so $benchmark_Command) &> $benchmarkCommand_outputDir/$benchmarkName/output.txt
    #(time $benchmark_Command) &> $benchmarkCommand_outputDir/$benchmarkName/execTime.txt
    outDir=${benchmarkCommand_outputDir}/${benchmarkName}/
    #sleep 30
#    cd /home/proy/benchmarks_result/rodinia/runBenchmarks/
    mv DataMalloc0.dat $outDir # DataMalloc${benchmarkName}_$6.dat
    mv DataPEBS0.dat  $outDir #DataPEBS${benchmarkName}_$6.dat
done
