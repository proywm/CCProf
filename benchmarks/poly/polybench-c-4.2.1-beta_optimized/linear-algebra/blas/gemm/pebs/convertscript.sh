/home/proy/cTool/MemTool/convert2 DataMalloc0.dat DataPEBS0.dat 1 > MemoryAllocationFree
/home/proy/cTool/MemTool/convert2 DataMalloc0.dat DataPEBS0.dat 0 > sampledAccess
sh userspace.sh
sh whichSetInL1.sh
sh plot.sh plot_overSet.r
sh getavgDav.sh
