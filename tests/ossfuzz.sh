#!/bin/bash -eu
# Copyright 2020 Google Inc.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
################################################################################

# This script is meant to be run by
# https://github.com/google/oss-fuzz/blob/master/projects/miniz/Dockerfile

cat << "EOF" > miniz_export.h
#ifndef MINIZ_EXPORT
#define MINIZ_EXPORT
#endif
EOF

mkdir build
cd build
cmake .. -DAMALGAMATE_SOURCES=ON -DBUILD_SHARED_LIBS=OFF -DBUILD_FUZZERS=ON -DBUILD_TESTS=OFF
make -j$(nproc)
cd ..

zip $OUT/seed_corpus.zip *.*

for f in $(find $SRC -name '*_fuzzer.c'); do
    b=$(basename -s .c $f)
    $CC $CFLAGS -Ibuild/amalgamation $f -c -o /tmp/$b.o
    $CXX $CXXFLAGS -stdlib=libc++ -Ibuild/amalgamation /tmp/$b.o -o $OUT/$b $LIB_FUZZING_ENGINE ./build/libminiz.a
    rm -f /tmp/$b.o
    ln -sf $OUT/seed_corpus.zip $OUT/${b}_seed_corpus.zip
done


# Add .zip input file for the zip fuzzer
rm -f $OUT/zip_fuzzer_seed_corpus.zip
zip $OUT/zip_fuzzer_seed_corpus.zip $OUT/seed_corpus.zip

cp tests/zip.dict $OUT/zip_fuzzer.dict
