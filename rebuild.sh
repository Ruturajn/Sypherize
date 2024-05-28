#!/usr/bin/env bash

set -eou pipefail

rm -rf ./build
mkdir build
cd build
cmake -DCMAKE_CXX_COMPILER=g++ ../
cmake --build .
cd ..
