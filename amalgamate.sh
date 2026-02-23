#!/bin/bash

set -e

mkdir -p amalgamation

OUTPUT_PREFIX=_build/amalgamation

cmake -H. -B_build -DAMALGAMATE_SOURCES=ON -G"Unix Makefiles"

cp $OUTPUT_PREFIX/miniz.* amalgamation/
cp ChangeLog.md amalgamation/
cp LICENSE amalgamation/
cp readme.md amalgamation/
mkdir -p amalgamation/examples
cp examples/* amalgamation/examples/

cd amalgamation
! test -e miniz.zip || rm miniz.zip
cat << EOF | zip -@ miniz
miniz.c
miniz.h
ChangeLog.md
LICENSE
readme.md
examples/example1.c
examples/example2.c
examples/example3.c
examples/example4.c
examples/example5.c
examples/example6.c
EOF
cd ..

echo "Amalgamation created."


