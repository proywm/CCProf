export CC=gcc
export CXX=g++
export FC=gfortran
export F77=gfortran
cd CCProf
sh papiBuildScript.sh
sh papiLibmonitorScript.sh
sh buildAnalyzer.sh
echo "export CCPROFDIR=$PWD" >> ~/.bashrc

cd LoopAnalyzer
git clone git://git.gnome.org/libxml2
cd libxml2
sh autogen.sh
make
