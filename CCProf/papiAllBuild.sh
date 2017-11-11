export CC=gcc
export CXX=g++
export FC=gfortran
export F77=gfortran
sh papiBuildScript.sh
sh papiLibmonitorScript.sh
echo "export CCPROFDIR=$PWD" >> ~/.bashrc
