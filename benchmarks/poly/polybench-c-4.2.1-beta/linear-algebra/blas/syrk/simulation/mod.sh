PIN_ROOT=/home/proy/cTool/pintool/pin-2.14-71313-gcc.4.4.7-linux
PIN_TOOL=$PIN_ROOT/source/tools/ManualExamples/obj-intel64/pinatrace.so
DIR=$PWD
$PIN_ROOT/pin.sh -t $PIN_TOOL -- $*
awk '{if($2=="R") print 0 " "$4; if($2=="W") print 1 " "$4;}'  pinatrace.out &> dinero_miss.din

awk '{if(!$2) {print "W";}else print $0;}' dinero_miss.din &> dinero_Miss.din1
sed -i.bak '/W\|R/d' ./dinero_Miss.din1
sh rundinero_mod.sh dinero_Miss.din1 > out_dinero_Miss.din
awk '{if($3==0)print $0 } END {; }' out_dinero_Miss.din > dinero_temp_miss
sed -e 's/,//g' dinero_temp_miss > tracedMisses
rm dinero_temp_miss
awk < tracedMisses '{print $5}' | sort | uniq -c | sort -nr > SetsInL1
#cat dinero_Miss.din | parallel --pipe sed -i.bak '/W/d' > dinero_Miss.din1
#sed -i.bak '/0 \n/d' ./dinero_conflictMiss.din
#sed -i.bak '/1 \n/d' ./dinero_conflictMiss.din
#awk '{if(!$2) {print "W";}else print $0;}'  dinero_conflictMiss.din &> dinero_conflictMiss.din3
#sed -i.bak '/delete/d' ./ dinero_conflictMiss.din
