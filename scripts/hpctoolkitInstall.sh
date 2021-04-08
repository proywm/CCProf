mkdir hpcToolKit
cd hpcToolKit
HPCTOOLKITDIR=$PWD

mkdir installDirExternal
cd installDirExternal
HPCTOOLKITDIR_INSTALL_DIR_EXTERNAL=$PWD
cd $HPCTOOLKITDIR
mkdir installDir
cd installDir
HPCTOOLKITDIR_INSTALL_DIR=$PWD
cd $HPCTOOLKITDIR

#git clone https://github.com/HPCToolkit/hpctoolkit.git
#git clone https://github.com/HPCToolkit/hpctoolkit-externals.git
git clone https://github.com/proywm/ccprof_hpctoolkit_deps.git

cd ccprof_hpctoolkit_deps/hpctoolkit-externals
mkdir BUILD && cd BUILD
../configure --prefix=$HPCTOOLKITDIR_INSTALL_DIR_EXTERNAL
make -j32
make install
make clean

cd $HPCTOOLKITDIR
cd ccprof_hpctoolkit_deps/hpctoolkit
mkdir BUILD && cd BUILD
../configure --prefix=$HPCTOOLKITDIR_INSTALL_DIR --with-externals=$HPCTOOLKITDIR_INSTALL_DIR_EXTERNAL
make -j32
make install

echo "export PATH=$HPCTOOLKITDIR_INSTALL_DIR/bin:$PATH" >> ~/.bashrc
echo "export LD_LIBRARY_PATH=$HPCTOOLKITDIR_INSTALL_DIR/lib:$LD_LIBRARY_PATH" >> ~/.bashrc

