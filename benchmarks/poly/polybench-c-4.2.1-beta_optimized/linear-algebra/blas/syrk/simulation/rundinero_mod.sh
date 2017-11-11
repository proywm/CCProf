DINARO_ROOT=/home/proy/cTool/dineroIV_mod/d4-7
#$DINARO_ROOT/dineroIV -l1-dsize 32k -l1-dbsize 64 -l1-dassoc 8 -l1-dccc -l1-dfetch a -informat d < $*
$DINARO_ROOT/dineroIV -l1-dsize 32k -l1-dbsize 64 -l1-dassoc 8 -l1-dccc -informat d < $*
