#!/usr/bin/env sh
rm -fr cget
rm -fr build
./init.sh
export DYLD_LIBRARY_PATH=$DYLD_LIBRARY_PATH:$(pwd)/cget/lib:$(pwd)/cget/lib64
mkdir build
cd build
cmake cmake .. -DBUILD_SHARED_LIB=ON ..
make
make test
