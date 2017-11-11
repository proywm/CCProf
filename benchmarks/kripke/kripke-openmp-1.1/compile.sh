rm -rf build
mkdir build
cd build
export CFLAGS="-g -O0"
export CXXFLAGS="-g -O0"
cmake ..
make -j8
