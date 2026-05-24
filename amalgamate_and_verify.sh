#!/bin/bash

set -e

source ./amalgamate.sh

echo "int main() { return 0; }" > main.c
echo "Test compile with GCC..."
gcc -pedantic -Wall -I$OUTPUT_PREFIX main.c $OUTPUT_PREFIX/miniz.c -o test.out
echo "Test compile with GCC ANSI..."
gcc -ansi -pedantic -Wall -I$OUTPUT_PREFIX main.c $OUTPUT_PREFIX/miniz.c -o test.out
if command -v clang
then
		echo "Test compile with clang..."
        clang -Wall -Wpedantic -fsanitize=unsigned-integer-overflow -I$OUTPUT_PREFIX main.c $OUTPUT_PREFIX/miniz.c -o test.out
fi
for def in MINIZ_NO_STDIO MINIZ_NO_TIME MINIZ_NO_DEFLATE_APIS MINIZ_NO_INFLATE_APIS MINIZ_NO_ARCHIVE_APIS MINIZ_NO_ARCHIVE_WRITING_APIS MINIZ_NO_ZLIB_APIS MINIZ_NO_ZLIB_COMPATIBLE_NAMES MINIZ_NO_MALLOC
do
	echo "Test compile with GCC and define $def..."
	gcc -ansi -pedantic -Wall -I$OUTPUT_PREFIX main.c $OUTPUT_PREFIX/miniz.c -o test.out -D${def}
done
echo "Test compile with GCC and MINIZ_USE_UNALIGNED_LOADS_AND_STORES=1..."
gcc -ansi -pedantic -Wall -I$OUTPUT_PREFIX main.c $OUTPUT_PREFIX/miniz.c -o test.out -DMINIZ_USE_UNALIGNED_LOADS_AND_STORES=1
rm test.out
rm main.c

echo "Amalgamation verified."



