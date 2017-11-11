DINARO_ROOT=/home/proy/cTool/dineroIV_mod/d4-7
#$DINARO_ROOT/dineroIV -l1-dsize 32k -l1-dbsize 64 -l1-dassoc 8 -l1-dccc -l1-dfetch a -informat d < $*
#$DINARO_ROOT/dineroIV -l1-dsize 32k -l1-dbsize 64 -l1-dassoc 8 -l1-dccc -informat d < $*
$DINARO_ROOT/dineroIV -l1-dsize 32k -l1-dbsize 32 -l1-dassoc 2  -l1-drepl l -l1-dccc -l1-dwalloc a -l1-dwback a -informat d < $*
