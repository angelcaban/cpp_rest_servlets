#!/bin/bash -e

mkdir build-debug
cd build-debug
CC=clang-6.0 CXX=clang++-6.0 cmake -G Ninja -DCMAKE_BUILD_TYPE=Debug -DCMAKE_INSTALL_PREFIX=../dist ..
ninja
./service_test
ninja install
