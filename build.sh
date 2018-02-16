#!/bin/bash

# build
mkdir build
cd build
cmake ..
make

# run tests
# ctest --output-on-failure