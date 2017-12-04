export CC=gcc
export CXX=g++
export FC=gfortran
export F77=gfortran
INSTALLDIR=$PWD

########################################### installing CCProf  #########################
cd CCProf
sh papiBuildScript.sh
sh papiLibmonitorScript.sh
sh buildAnalyzer.sh
echo "export CCPROFDIR=$PWD" >> ~/.bashrc

cd LoopAnalyzer
git clone git://git.gnome.org/libxml2
cd libxml2
sh autogen.sh
make -j8
cd ..
make -j8


######################################### installing hpctoolkit  #######################

cd $INSTALLDIR
#now install hpctoolkit
sh scripts/hpctoolkitInstall.sh


####################################### installing python modules ######################

#cd $INSTALLDIR
#sudo python scripts/pythonDependency.py
