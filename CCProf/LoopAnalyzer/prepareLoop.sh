$CCPROFDIR/LoopAnalyzer/treeXtract $* > loopout
awk -v var=$* '{printf "%d\t%d\t%d\t%d\t%d\t%d\n",$2,$4,$5,$7,$8,$9}' loopout > loopsAll
python findinnerloops.py > loopsTemp
awk -v var=$* '{printf "%d\t%d\t%d\t%d\n",$2,$6,$7,$8}' loopsTemp > loops
awk -v var=$* '{if($5==1) printf "0x%x\n",$8}' loopout > LoopAddresses

