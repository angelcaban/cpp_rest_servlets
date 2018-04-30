#!/bin/bash -e

mkdir build-debug
cd build-debug
cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_INSTALL_PREFIX=../dist ..
make -j
./service_test
make install
