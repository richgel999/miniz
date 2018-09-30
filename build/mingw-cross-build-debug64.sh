#!/bin/sh

    cd ../
    sudo -u admin svn update
    cd build

    mkdir -p mingw64
    cd mingw64
    mingw64-cmake ../../ -DCMAKE_BUILD_TYPE=Debug
    mingw64-make
    cd ..
    rm -rf mingw64
