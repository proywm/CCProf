PAPIDIR=/home/proy/cTool/MemTool/PAPI/PAPI
#export LD_LIBRARY_PATH=$PAPIDIR/src/libpfm4/lib

#LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/home/proy/cTool/MemTool/PAPI/PAPI/src/libpfm4/lib /home/proy/cTool/MemTool/libmonitor/libmonitor/installDir/bin/monitor-run -i /home/proy/cTool/MemTool/libPapiMonitor.so -d /home/proy/cTool/scripts/temp/testPLDIBENCH/pldi_sriram_bench/polybench-c-4.2.1-beta/linear-algebra/blas/gemm/gemm

sh /home/proy/cTool/MemTool/memtoolrun.sh /home/proy/cTool/scripts/temp/testPLDIBENCH/pldi_sriram_bench/polybench-c-4.2.1-beta/linear-algebra/blas/gemm/gemm
