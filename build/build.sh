#!/bin/bash

    cd ../
    sudo -u admin svn update
    cd build

    mkdir -p linux
    cd linux
    cmake -G"Unix Makefiles" ../../ -DCMAKE_BUILD_TYPE=Debug
    cmake --build . --config "Debug"
    cd ..
    rm -rf linux

    rm -rf ../bin
