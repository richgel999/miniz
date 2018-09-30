#!/bin/sh

    cd ../
    sudo -u admin svn update
    cd build

    # Auto in cmake
    # cross compile flag required: -D__CROSS_COMPILE_TIME__=1
    #
    mkdir -p mingw32
    cd mingw32
    mingw32-cmake ../../ -DCMAKE_BUILD_TYPE=Debug
    mingw32-make
    cd ..
    rm -rf mingw32

    mkdir -p mingw64
    cd mingw64
    mingw64-cmake ../../ -DCMAKE_BUILD_TYPE=Debug
    mingw64-make
    cd ..
    rm -rf mingw64
