# Introduction #

This page shows how miniz.c v1.10 compares to several other popular open source lossless data compression codecs at compression level 9 (max compression), with two examples at level 1 (fastest compression) on enwik8 and enwik9. The data here was generated using a slightly modified and enhanced version of [John Ratcliff's](http://en.wikipedia.org/wiki/John_W._Ratcliff) [compressiontest](http://code.google.com/p/compressiontest/) project, compiled to x64.

On a Core i7 3.2 GHz, miniz.c's typical decompression rate is around 175-250 MB/sec., and its typical compression rate is anywhere from 14-120.5MB/s (actual rates depend on the compression level, as well as the compressibility and redundancy present in the source data). The x64 version of the decompressor is faster than the x86 version, sometimes up to 20%, mostly due to good register utilization. miniz.c's inflater can be optionally configured to use a 64-bit bitbuffer on 64-bit CPU's, and unaligned 16 and 32-bit loads on little endian platforms (the tests below had both optimizations enabled).

For comparison purposes, I've added [stb\_image.c](http://nothings.org/stb_image.c)'s Inflater implementation to compresssiontest, and [LZHAM](http://code.google.com/p/lzham/) alpha7. Note that miniz.c is used for compressing the data supplied to stb\_image.c for decompression (stb\_image.c does not include a compressor, [stb\_image\_write.h](http://nothings.org/stb/stb_image_write.h) contains a simple compressor but I haven't tested it yet). Also, stb\_image.c does not compute an [Adler-32](http://en.wikipedia.org/wiki/Adler32) checksum of the uncompressed data, and miniz.c and zlib do, so stb's actual "apples to apples" performance is a little lower than reported here. (Approximately 10% of miniz.c's decompression time is spent computing the Adler32 checksum. Note, stb\_image.c is extremely useful and it inspired miniz.c -- I'm only using it as a point of reference to help compare alternate Inflate implementations against zlib.)

Test machine: Win7 Ultimate x64, Intel Gulftown Core i7 3.2GHz (6 cores, 12 hyperthreads), 24GB of RAM, EVGA X58 motherboard. All code was compiled with Visual Studio 2005.

LZHAM and LZMA where both set to use 512MB dictionary sizes in these tests.

**File: [enwik8](http://cs.fit.edu/~mmahoney/compression/textdata.html) (miniz.c compression level 9):**
```
E:\lzham\compressiontest\test_compression>test_compression_x64.exe E:\dev\corpus\enwik8\enwik8
x64 version
Reading test file 'E:\dev\corpus\enwik8\enwik8' which is 100,000,000 bytes long.

---------------------------------------------------------------
Testing Compression rate and speed with various compressors.
---------------------------------------------------------------
Compress:CT_STB     :FROM:100,000,000 TO: 36,460,117 63.54%    5,871 MS
Compress:CT_MINIZ   :FROM:100,000,000 TO: 36,460,117 63.54%    5,872 MS
Compress:CT_CRYPTO  :FROM:100,000,000 TO: 36,642,772 63.36%    7,501 MS
Compress:CT_MINILZO :FROM:100,000,000 TO: 53,481,960 46.52%      868 MS
Compress:CT_ZLIB    :FROM:100,000,000 TO: 36,475,808 63.52%    7,635 MS
Compress:CT_BZIP    :FROM:100,000,000 TO: 33,259,584 66.74%    9,444 MS
Compress:CT_LIBLZF  :FROM:100,000,000 TO: 53,945,398 46.05%      688 MS
Compress:CT_LZMA    :FROM:100,000,000 TO: 24,797,518 75.20%   86,542 MS
Compress:CT_FASTLZ  :FROM:100,000,000 TO: 54,163,029 45.84%      738 MS
Compress:CT_LZHAM   :FROM:100,000,000 TO: 24,814,835 75.19%   29,200 MS

---------------------------------------------------------------
Testing Decompression speed with various decompressors.
---------------------------------------------------------------
Decompress:CT_STB     :FROM: 36,460,117 TO:100,000,000      731 MS
Decompress:CT_MINIZ   :FROM: 36,460,117 TO:100,000,000      559 MS
Decompress:CT_CRYPTO  :FROM: 36,642,772 TO:100,000,000    1,823 MS
Decompress:CT_MINILZO :FROM: 53,481,960 TO:100,000,000      463 MS
Decompress:CT_ZLIB    :FROM: 36,475,808 TO:100,000,000      570 MS
Decompress:CT_BZIP    :FROM: 33,259,584 TO:100,000,000    3,587 MS
Decompress:CT_LIBLZF  :FROM: 53,945,398 TO:100,000,000      429 MS
Decompress:CT_LZMA    :FROM: 24,797,518 TO:100,000,000    1,492 MS
Decompress:CT_FASTLZ  :FROM: 54,163,029 TO:100,000,000      458 MS
Decompress:CT_LZHAM   :FROM: 24,814,835 TO:100,000,000      848 MS
```

**File: [enwik8](http://cs.fit.edu/~mmahoney/compression/textdata.html) (miniz.c compression level 1 (MZ\_BEST\_SPEED), zlib set to Z\_BEST\_SPEED, all other settings are the same as above):**
```
E:\lzham\compressiontest\test_compression>test_compression_x64.exe E:\dev\corpus\enwik8\enwik8
x64 version
Reading test file 'E:\dev\corpus\enwik8\enwik8' which is 100,000,000 bytes long.

---------------------------------------------------------------
Testing Compression rate and speed with various compressors.
---------------------------------------------------------------
Compress:CT_STB     :FROM:100,000,000 TO: 48,510,044 51.49%    1,159 MS
Compress:CT_MINIZ   :FROM:100,000,000 TO: 48,510,044 51.49%    1,158 MS
Compress:CT_CRYPTO  :FROM:100,000,000 TO: 36,642,772 63.36%    7,501 MS
Compress:CT_MINILZO :FROM:100,000,000 TO: 53,481,960 46.52%      868 MS
Compress:CT_ZLIB    :FROM:100,000,000 TO: 42,298,790 57.70%    2,443 MS
Compress:CT_BZIP    :FROM:100,000,000 TO: 33,259,584 66.74%    9,431 MS
Compress:CT_LIBLZF  :FROM:100,000,000 TO: 53,945,398 46.05%      685 MS
Compress:CT_LZMA    :FROM:100,000,000 TO: 24,797,518 75.20%   86,736 MS
Compress:CT_FASTLZ  :FROM:100,000,000 TO: 54,163,029 45.84%      739 MS
Compress:CT_LZHAM   :FROM:100,000,000 TO: 24,813,424 75.19%   29,210 MS

---------------------------------------------------------------
Testing Decompression speed with various decompressors.
---------------------------------------------------------------
Decompress:CT_STB     :FROM: 48,510,044 TO:100,000,000      987 MS
Decompress:CT_MINIZ   :FROM: 48,510,044 TO:100,000,000      718 MS
Decompress:CT_CRYPTO  :FROM: 36,642,772 TO:100,000,000    1,823 MS
Decompress:CT_MINILZO :FROM: 53,481,960 TO:100,000,000      463 MS
Decompress:CT_ZLIB    :FROM: 42,298,790 TO:100,000,000      612 MS
Decompress:CT_BZIP    :FROM: 33,259,584 TO:100,000,000    3,563 MS
Decompress:CT_LIBLZF  :FROM: 53,945,398 TO:100,000,000      429 MS
Decompress:CT_LZMA    :FROM: 24,797,518 TO:100,000,000    1,497 MS
Decompress:CT_FASTLZ  :FROM: 54,163,029 TO:100,000,000      458 MS
Decompress:CT_LZHAM   :FROM: 24,813,424 TO:100,000,000      853 MS
```

**File: [enwik9](http://cs.fit.edu/~mmahoney/compression/textdata.html) (miniz.c compression level 9):**
```
E:\lzham\compressiontest\test_compression>test_compression_x64.exe E:\dev\corpus\enwik9\enwik9
x64 version
Reading test file 'E:\dev\corpus\enwik9\enwik9' which is 1,000,000,000 bytes long.

---------------------------------------------------------------
Testing Compression rate and speed with various compressors.
---------------------------------------------------------------
Compress:CT_STB     :FROM:1,000,000,000 TO:322,771,589 67.72%   50,859 MS
Compress:CT_MINIZ   :FROM:1,000,000,000 TO:322,771,589 67.72%   50,869 MS
Compress:CT_CRYPTO  :FROM:1,000,000,000 TO:324,897,277 67.51%   65,744 MS
Compress:CT_MINILZO :FROM:1,000,000,000 TO:477,237,740 52.28%    7,807 MS
Compress:CT_ZLIB    :FROM:1,000,000,000 TO:322,789,246 67.72%   66,571 MS
Compress:CT_BZIP    :FROM:1,000,000,000 TO:295,663,950 70.43%   94,037 MS
Compress:CT_LIBLZF  :FROM:1,000,000,000 TO:492,987,206 50.70%    6,310 MS
Compress:CT_LZMA    :FROM:1,000,000,000 TO:201,776,574 79.82% 1,043,293 MS
Compress:CT_FASTLZ  :FROM:1,000,000,000 TO:487,260,768 51.27%    6,748 MS
Compress:CT_LZHAM   :FROM:1,000,000,000 TO:202,475,460 79.75%  351,687 MS

---------------------------------------------------------------
Testing Decompression speed with various decompressors.
---------------------------------------------------------------
Decompress:CT_STB     :FROM:322,771,589 TO:1,000,000,000    6,717 MS
Decompress:CT_MINIZ   :FROM:322,771,589 TO:1,000,000,000    5,050 MS
Decompress:CT_CRYPTO  :FROM:324,897,277 TO:1,000,000,000   16,770 MS
Decompress:CT_MINILZO :FROM:477,237,740 TO:1,000,000,000    4,205 MS
Decompress:CT_ZLIB    :FROM:322,789,246 TO:1,000,000,000    5,171 MS
Decompress:CT_BZIP    :FROM:295,663,950 TO:1,000,000,000   33,298 MS
Decompress:CT_LIBLZF  :FROM:492,987,206 TO:1,000,000,000    4,025 MS
Decompress:CT_LZMA    :FROM:201,776,574 TO:1,000,000,000   12,650 MS
Decompress:CT_FASTLZ  :FROM:487,260,768 TO:1,000,000,000    4,223 MS
Decompress:CT_LZHAM   :FROM:202,475,460 TO:1,000,000,000    7,444 MS
```

**File: [enwik9](http://cs.fit.edu/~mmahoney/compression/textdata.html) (miniz.c compression level 1 (MZ\_BEST\_SPEED), zlib set to Z\_BEST\_SPEED, all other settings are the same as above):**
```
E:\lzham\compressiontest\test_compression>test_compression_x64.exe E:\dev\corpus\enwik9\enwik9
x64 version
Reading test file 'E:\dev\corpus\enwik9\enwik9' which is 1,000,000,000 bytes long.

---------------------------------------------------------------
Testing Compression rate and speed with various compressors.
---------------------------------------------------------------
Compress:CT_STB     :FROM:1,000,000,000 TO:436,564,069 56.34%   10,417 MS
Compress:CT_MINIZ   :FROM:1,000,000,000 TO:436,564,069 56.34%   10,418 MS
Compress:CT_CRYPTO  :FROM:1,000,000,000 TO:324,897,277 67.51%   66,169 MS
Compress:CT_MINILZO :FROM:1,000,000,000 TO:477,237,740 52.28%    7,769 MS
Compress:CT_ZLIB    :FROM:1,000,000,000 TO:378,355,092 62.16%   21,900 MS
Compress:CT_BZIP    :FROM:1,000,000,000 TO:295,663,950 70.43%   93,694 MS
Compress:CT_LIBLZF  :FROM:1,000,000,000 TO:492,987,206 50.70%    6,326 MS
Compress:CT_LZMA    :FROM:1,000,000,000 TO:201,776,574 79.82% 1,043,293 MS
Compress:CT_FASTLZ  :FROM:1,000,000,000 TO:487,260,768 51.27%    6,756 MS
Compress:CT_LZHAM   :FROM:1,000,000,000 TO:202,475,460 79.75%  351,687 MS

---------------------------------------------------------------
Testing Decompression speed with various decompressors.
---------------------------------------------------------------
Decompress:CT_STB     :FROM:436,564,069 TO:1,000,000,000    9,010 MS
Decompress:CT_MINIZ   :FROM:436,564,069 TO:1,000,000,000    6,530 MS
Decompress:CT_CRYPTO  :FROM:324,897,277 TO:1,000,000,000   16,786 MS
Decompress:CT_MINILZO :FROM:477,237,740 TO:1,000,000,000    4,205 MS
Decompress:CT_ZLIB    :FROM:378,355,092 TO:1,000,000,000    5,612 MS
Decompress:CT_BZIP    :FROM:295,663,950 TO:1,000,000,000   33,340 MS
Decompress:CT_LIBLZF  :FROM:492,987,206 TO:1,000,000,000    3,977 MS
Decompress:CT_LZMA    :FROM:201,776,574 TO:1,000,000,000   12,650 MS
Decompress:CT_FASTLZ  :FROM:487,260,768 TO:1,000,000,000    4,214 MS
Decompress:CT_LZHAM   :FROM:202,475,460 TO:1,000,000,000    7,444 MS
```


**File: Entire Large [Calgary Corpus](http://www.data-compression.info/Corpora/CalgaryCorpus/) added to a single uncompressed 7zip archive (miniz.c compression level 9):**
```
E:\lzham\compressiontest\test_compression>test_compression_x64.exe e:\dev\corpus\calgary_corpus.7z
x64 version
Reading test file 'e:\dev\corpus\calgary_corpus.7z' which is 3,251,898 bytes long.

---------------------------------------------------------------
Testing Compression rate and speed with various compressors.
---------------------------------------------------------------
Compress:CT_STB     :FROM:  3,251,898 TO:  1,065,470 67.24%      224 MS
Compress:CT_MINIZ   :FROM:  3,251,898 TO:  1,065,470 67.24%      224 MS
Compress:CT_CRYPTO  :FROM:  3,251,898 TO:  1,067,241 67.18%      240 MS
Compress:CT_MINILZO :FROM:  3,251,898 TO:  1,581,684 51.36%       26 MS
Compress:CT_ZLIB    :FROM:  3,251,898 TO:  1,059,243 67.43%      353 MS
Compress:CT_BZIP    :FROM:  3,251,898 TO:    961,353 70.44%      277 MS
Compress:CT_LIBLZF  :FROM:  3,251,898 TO:  1,593,245 51.01%       20 MS
Compress:CT_LZMA    :FROM:  3,251,898 TO:    853,241 73.76%    1,570 MS
Compress:CT_FASTLZ  :FROM:  3,251,898 TO:  1,606,374 50.60%       21 MS
Compress:CT_LZHAM   :FROM:  3,251,898 TO:    893,578 72.52%    1,028 MS

---------------------------------------------------------------
Testing Decompression speed with various decompressors.
---------------------------------------------------------------
Decompress:CT_STB     :FROM:  1,065,470 TO:  3,251,898       22 MS
Decompress:CT_MINIZ   :FROM:  1,065,470 TO:  3,251,898       17 MS
Decompress:CT_CRYPTO  :FROM:  1,067,241 TO:  3,251,898       54 MS
Decompress:CT_MINILZO :FROM:  1,581,684 TO:  3,251,898       14 MS
Decompress:CT_ZLIB    :FROM:  1,059,243 TO:  3,251,898       17 MS
Decompress:CT_BZIP    :FROM:    961,353 TO:  3,251,898      104 MS
Decompress:CT_LIBLZF  :FROM:  1,593,245 TO:  3,251,898       13 MS
Decompress:CT_LZMA    :FROM:    853,241 TO:  3,251,898       55 MS
Decompress:CT_FASTLZ  :FROM:  1,606,374 TO:  3,251,898       13 MS
Decompress:CT_LZHAM   :FROM:    893,578 TO:  3,251,898       39 MS
```

**File: "pic" from the Large [Calgary Corpus](http://www.data-compression.info/Corpora/CalgaryCorpus/) (miniz.c compression level 9):**
```
E:\lzham\compressiontest\test_compression>test_compression_x64.exe e:\dev\corpus\large_calgary_corpus\pic
x64 version
Reading test file 'e:\dev\corpus\large_calgary_corpus\pic' which is 513,216 bytes long.

---------------------------------------------------------------
Testing Compression rate and speed with various compressors.
---------------------------------------------------------------
Compress:CT_STB     :FROM:    513,216 TO:     53,721 89.53%       26 MS
Compress:CT_MINIZ   :FROM:    513,216 TO:     53,721 89.53%       26 MS
Compress:CT_CRYPTO  :FROM:    513,216 TO:     57,166 88.86%       18 MS
Compress:CT_MINILZO :FROM:    513,216 TO:     86,304 83.18%        2 MS
Compress:CT_ZLIB    :FROM:    513,216 TO:     52,237 89.82%       96 MS
Compress:CT_BZIP    :FROM:    513,216 TO:     49,928 90.27%       17 MS
Compress:CT_LIBLZF  :FROM:    513,216 TO:     80,772 84.26%        1 MS
Compress:CT_LZMA    :FROM:    513,216 TO:     41,897 91.84%      367 MS
Compress:CT_FASTLZ  :FROM:    513,216 TO:     81,960 84.03%        1 MS
Compress:CT_LZHAM   :FROM:    513,216 TO:     47,437 90.76%      515 MS

---------------------------------------------------------------
Testing Decompression speed with various decompressors.
---------------------------------------------------------------
Decompress:CT_STB     :FROM:     53,721 TO:    513,216        2 MS
Decompress:CT_MINIZ   :FROM:     53,721 TO:    513,216        2 MS
Decompress:CT_CRYPTO  :FROM:     57,166 TO:    513,216        4 MS
Decompress:CT_MINILZO :FROM:     86,304 TO:    513,216        2 MS
Decompress:CT_ZLIB    :FROM:     52,237 TO:    513,216        1 MS
Decompress:CT_BZIP    :FROM:     49,928 TO:    513,216        6 MS
Decompress:CT_LIBLZF  :FROM:     80,772 TO:    513,216        1 MS
Decompress:CT_LZMA    :FROM:     41,897 TO:    513,216        4 MS
Decompress:CT_FASTLZ  :FROM:     81,960 TO:    513,216        1 MS
Decompress:CT_LZHAM   :FROM:     47,437 TO:    513,216        7 MS
```

**Files: Uncompressed 7zip archive of an Adobe Acrobat Reader 9.0 installation (miniz.c compression level 9):**
```
E:\lzham\compressiontest\test_compression>test_compression_x64.exe e:\dev\corpus\adobe.7z
x64 version
Reading test file 'e:\dev\corpus\adobe.7z' which is 146,332,683 bytes long.

---------------------------------------------------------------
Testing Compression rate and speed with various compressors.
---------------------------------------------------------------
Compress:CT_STB     :FROM:146,332,683 TO: 89,281,876 38.99%    9,780 MS
Compress:CT_MINIZ   :FROM:146,332,683 TO: 89,281,876 38.99%    9,781 MS
Compress:CT_CRYPTO  :FROM:146,332,683 TO: 91,494,418 37.48%    7,156 MS
Compress:CT_MINILZO :FROM:146,332,683 TO:101,687,569 30.51%    1,702 MS
Compress:CT_ZLIB    :FROM:146,332,683 TO: 89,166,466 39.07%   13,143 MS
Compress:CT_BZIP    :FROM:146,332,683 TO: 89,072,636 39.13%   20,828 MS
Compress:CT_LIBLZF  :FROM:146,332,683 TO:106,352,717 27.32%    1,100 MS
Compress:CT_LZMA    :FROM:146,332,683 TO: 74,361,137 49.18%   59,076 MS
Compress:CT_FASTLZ  :FROM:146,332,683 TO:105,548,437 27.87%    1,032 MS
Compress:CT_LZHAM   :FROM:146,332,683 TO: 75,314,840 48.53%   42,354 MS

---------------------------------------------------------------
Testing Decompression speed with various decompressors.
---------------------------------------------------------------
Decompress:CT_STB     :FROM: 89,281,876 TO:146,332,683    1,119 MS
Decompress:CT_MINIZ   :FROM: 89,281,876 TO:146,332,683      773 MS
Decompress:CT_CRYPTO  :FROM: 91,494,418 TO:146,332,683    2,142 MS
Decompress:CT_MINILZO :FROM:101,687,569 TO:146,332,683      545 MS
Decompress:CT_ZLIB    :FROM: 89,166,466 TO:146,332,683      820 MS
Decompress:CT_BZIP    :FROM: 89,072,636 TO:146,332,683    7,101 MS
Decompress:CT_LIBLZF  :FROM:106,352,717 TO:146,332,683      565 MS
Decompress:CT_LZMA    :FROM: 74,361,137 TO:146,332,683    5,259 MS
Decompress:CT_FASTLZ  :FROM:105,548,437 TO:146,332,683      573 MS
Decompress:CT_LZHAM   :FROM: 75,314,840 TO:146,332,683    1,761 MS
```

**File: miniz3\_v109.zip (a test of how well various codecs handle already compressed data) (miniz.c compression level 9):**
```
E:\lzham\compressiontest\test_compression>test_compression_x64.exe e:\dev\miniz3_v109.zip
x64 version
Reading test file 'e:\dev\miniz3_v109.zip' which is 417,549 bytes long.

---------------------------------------------------------------
Testing Compression rate and speed with various compressors.
---------------------------------------------------------------
Compress:CT_STB     :FROM:    417,549 TO:    416,312 0.30%       17 MS
Compress:CT_MINIZ   :FROM:    417,549 TO:    416,312 0.30%       17 MS
Compress:CT_CRYPTO  :FROM:    417,549 TO:    417,658 -0.03%        9 MS
Compress:CT_MINILZO :FROM:    417,549 TO:    417,760 -0.05%        7 MS
Compress:CT_ZLIB    :FROM:    417,549 TO:    416,408 0.27%       18 MS
Compress:CT_BZIP    :FROM:    417,549 TO:    420,125 -0.62%       88 MS
Compress:CT_LIBLZF  :FROM:    417,549 TO:    429,060 -2.76%        4 MS
Compress:CT_LZMA    :FROM:    417,549 TO:    420,196 -0.63%      313 MS
Compress:CT_FASTLZ  :FROM:    417,549 TO:    429,044 -2.75%        3 MS
Compress:CT_LZHAM   :FROM:    417,549 TO:    417,575 -0.01%       82 MS

---------------------------------------------------------------
Testing Decompression speed with various decompressors.
---------------------------------------------------------------
Decompress:CT_STB     :FROM:    416,312 TO:    417,549        2 MS
Decompress:CT_MINIZ   :FROM:    416,312 TO:    417,549        1 MS
Decompress:CT_CRYPTO  :FROM:    417,658 TO:    417,549        2 MS
Decompress:CT_MINILZO :FROM:    417,760 TO:    417,549        1 MS
Decompress:CT_ZLIB    :FROM:    416,408 TO:    417,549        2 MS
Decompress:CT_BZIP    :FROM:    420,125 TO:    417,549       28 MS
Decompress:CT_LIBLZF  :FROM:    429,060 TO:    417,549        2 MS
Decompress:CT_LZMA    :FROM:    420,196 TO:    417,549       29 MS
Decompress:CT_FASTLZ  :FROM:    429,044 TO:    417,549        1 MS
Decompress:CT_LZHAM   :FROM:    417,575 TO:    417,549        2 MS
```