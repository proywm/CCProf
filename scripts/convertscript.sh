$CCPROFDIR/convert2 DataMalloc0.dat DataPEBS0.dat 1 > MemoryAllocationFree
$CCPROFDIR/convert2 DataMalloc0.dat DataPEBS0.dat 0 > sampledAccess
sh $CCPROFDIR/LoopAnalyzer/prepareLoop.sh *.hpcstruct
