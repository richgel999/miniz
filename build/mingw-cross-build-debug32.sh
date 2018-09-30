#!/bin/sh

    cd ../
    sudo -u admin svn update
    cd build

    mkdir -p mingw32
    cd mingw32
    mingw32-cmake ../../ -DCMAKE_BUILD_TYPE=Debug
    mingw32-make
    cd ..
    rm -rf mingw32
