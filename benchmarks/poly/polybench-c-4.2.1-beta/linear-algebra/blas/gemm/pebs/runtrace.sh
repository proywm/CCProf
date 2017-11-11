PIN_ROOT=/home/proy/cTool/pintool/pin-2.14-71313-gcc.4.4.7-linux
PIN_TOOL=$PIN_ROOT/source/tools/ManualExamples/obj-intel64/pinatrace.so
DIR=$PWD
$PIN_ROOT/pin.sh -t $PIN_TOOL -- $*
awk -F' ' '{if($2=="R") print 0 " "$3; else print 1 " "$3;}'  pinatrace.out &> dinaro.din
sed -i -e 's/0x//g' dinaro.din
