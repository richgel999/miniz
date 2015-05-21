# Introduction #

This page shows how miniz.c v1.09 compares to several other popular open source data compression codecs at compression level 9 (max compression), with one example at level 1 (fastest compression) on wik8. The data here was generated using a slightly modified and enhanced version of [John Ratcliff's](http://en.wikipedia.org/wiki/John_W._Ratcliff) [compressiontest](http://code.google.com/p/compressiontest/) project, compiled to x64.

miniz.c's typical decompression rate is around 175-250 MB/sec. on a Core i7, and its typical compression rate is anywhere from 7-48.5MB/s (actual rates depend on the compression level, as well as the compressibility and redundancy present in the source data). The x64 version of the decompressor is faster than the x86 version, sometimes up to 20%, mostly due to good register utilization. miniz.c's inflater can be optionally configured to use a 64-bit bitbuffer on 64-bit CPU's, and unaligned 16 and 32-bit loads on little endian platforms (the tests below had both optimizations enabled).

For comparison purposes, I've added [stb\_image.c](http://nothings.org/stb_image.c)'s Inflater implementation to compresssiontest, and [LZHAM](http://code.google.com/p/lzham/) alpha7. Note that miniz.c is used for compressing the data supplied to stb\_image.c for decompression (stb\_image.c does not include a compressor, [stb\_image\_write.h](http://nothings.org/stb/stb_image_write.h) contains a simple compressor but I haven't tested it yet). Also, stb\_image.c does not compute an [Adler-32](http://en.wikipedia.org/wiki/Adler32) checksum of the uncompressed data, and miniz.c and zlib do, so stb's actual "apples to apples" performance is a little lower than reported here. (Approximately 10% of miniz.c's decompression time is spent computing the Adler32 checksum. Note, stb\_image.c is extremely useful and it inspired miniz.c -- I'm only using it as a point of reference to help compare alternate Inflate implementations against zlib.)

Test machine: Win7 Ultimate x64, Intel Gulftown Core i7 3.2GHz (6 cores, 12 hyperthreads), 24GB of RAM, EVGA X58 motherboard. All code was compiled with Visual Studio 2005.

**File: [enwik8](http://cs.fit.edu/~mmahoney/compression/textdata.html) (miniz.c compression level 9):**
```
E:\lzham\compressiontest\test_compression>test_compression_x64 e:\dev\corpus\enwik8\enwik8
x64 version
Reading test file 'e:\dev\corpus\enwik8\enwik8' which is 100,000,000 bytes long.

---------------------------------------------------------------
Testing Compression rate and speed with various compressors.
---------------------------------------------------------------
Compress:CT_STB     :FROM:100,000,000 TO: 36,473,214 63.53%    6,398 MS
Compress:CT_MINIZ   :FROM:100,000,000 TO: 36,473,214 63.53%    6,394 MS
Compress:CT_CRYPTO  :FROM:100,000,000 TO: 36,642,772 63.36%    7,519 MS
Compress:CT_MINILZO :FROM:100,000,000 TO: 53,481,960 46.52%      870 MS
Compress:CT_ZLIB    :FROM:100,000,000 TO: 36,475,808 63.52%    7,620 MS
Compress:CT_BZIP    :FROM:100,000,000 TO: 33,259,584 66.74%    9,406 MS
Compress:CT_LIBLZF  :FROM:100,000,000 TO: 53,945,398 46.05%      688 MS
Compress:CT_LZMA    :FROM:100,000,000 TO: 25,209,489 74.79%   79,688 MS
Compress:CT_FASTLZ  :FROM:100,000,000 TO: 54,163,029 45.84%      738 MS
Compress:CT_LZHAM   :FROM:100,000,000 TO: 25,248,714 74.75%   26,848 MS

---------------------------------------------------------------
Testing Decompression speed with various decompressors.
---------------------------------------------------------------
Decompress:CT_STB     :FROM: 36,473,214 TO:100,000,000      728 MS
Decompress:CT_MINIZ   :FROM: 36,473,214 TO:100,000,000      558 MS
Decompress:CT_CRYPTO  :FROM: 36,642,772 TO:100,000,000    1,819 MS
Decompress:CT_MINILZO :FROM: 53,481,960 TO:100,000,000      467 MS
Decompress:CT_ZLIB    :FROM: 36,475,808 TO:100,000,000      571 MS
Decompress:CT_BZIP    :FROM: 33,259,584 TO:100,000,000    3,617 MS
Decompress:CT_LIBLZF  :FROM: 53,945,398 TO:100,000,000      428 MS
Decompress:CT_LZMA    :FROM: 25,209,489 TO:100,000,000    1,505 MS
Decompress:CT_FASTLZ  :FROM: 54,163,029 TO:100,000,000      455 MS
Decompress:CT_LZHAM   :FROM: 25,248,714 TO:100,000,000      853 MS
```

**File: [enwik8](http://cs.fit.edu/~mmahoney/compression/textdata.html) (miniz.c compression level 1), compares miniz and stb\_image.c only (all other results are the same as above):**
```
E:\lzham\compressiontest\test_compression>test_compression_x64.exe e:\dev\corpus\enwik8\enwik8
x64 version - miniz level 1 (fastest)
Reading test file 'e:\dev\corpus\enwik8\enwik8' which is 100,000,000 bytes long.

---------------------------------------------------------------
Testing Compression rate and speed with various compressors.
---------------------------------------------------------------
Compress:CT_STB     :FROM:100,000,000 TO: 40,669,733 59.33%    2,061 MS
Compress:CT_MINIZ   :FROM:100,000,000 TO: 40,669,733 59.33%    2,061 MS

---------------------------------------------------------------
Testing Decompression speed with various decompressors.
---------------------------------------------------------------
Decompress:CT_STB     :FROM: 40,669,733 TO:100,000,000      810 MS
Decompress:CT_MINIZ   :FROM: 40,669,733 TO:100,000,000      616 MS
```

**File: [enwik9](http://cs.fit.edu/~mmahoney/compression/textdata.html) (miniz.c compression level 9):**
```
E:\lzham\compressiontest\test_compression>test_compression_x64 e:\dev\corpus\enwik9\enwik9
x64 version
Reading test file 'e:\dev\corpus\enwik9\enwik9' which is 1,000,000,000 bytes long.

---------------------------------------------------------------
Testing Compression rate and speed with various compressors.
---------------------------------------------------------------
Compress:CT_STB     :FROM:1,000,000,000 TO:322,952,682 67.70%   55,295 MS
Compress:CT_MINIZ   :FROM:1,000,000,000 TO:322,952,682 67.70%   55,259 MS
Compress:CT_CRYPTO  :FROM:1,000,000,000 TO:324,897,277 67.51%   65,783 MS
Compress:CT_MINILZO :FROM:1,000,000,000 TO:477,237,740 52.28%    7,762 MS
Compress:CT_ZLIB    :FROM:1,000,000,000 TO:322,789,246 67.72%   66,350 MS
Compress:CT_BZIP    :FROM:1,000,000,000 TO:295,663,950 70.43%   93,238 MS
Compress:CT_LIBLZF  :FROM:1,000,000,000 TO:492,987,206 50.70%    6,306 MS
Compress:CT_LZMA    :FROM:1,000,000,000 TO:218,545,630 78.15%  748,686 MS
Compress:CT_FASTLZ  :FROM:1,000,000,000 TO:487,260,768 51.27%    6,740 MS
Compress:CT_LZHAM   :FROM:1,000,000,000 TO:219,044,419 78.10%  268,488 MS

---------------------------------------------------------------
Testing Decompression speed with various decompressors.
---------------------------------------------------------------
Decompress:CT_STB     :FROM:322,952,682 TO:1,000,000,000    6,704 MS
Decompress:CT_MINIZ   :FROM:322,952,682 TO:1,000,000,000    5,055 MS
Decompress:CT_CRYPTO  :FROM:324,897,277 TO:1,000,000,000   16,761 MS
Decompress:CT_MINILZO :FROM:477,237,740 TO:1,000,000,000    4,252 MS
Decompress:CT_ZLIB    :FROM:322,789,246 TO:1,000,000,000    5,180 MS
Decompress:CT_BZIP    :FROM:295,663,950 TO:1,000,000,000   33,624 MS
Decompress:CT_LIBLZF  :FROM:492,987,206 TO:1,000,000,000    4,019 MS
Decompress:CT_LZMA    :FROM:218,545,630 TO:1,000,000,000   13,300 MS
Decompress:CT_FASTLZ  :FROM:487,260,768 TO:1,000,000,000    4,208 MS
Decompress:CT_LZHAM   :FROM:219,044,419 TO:1,000,000,000    7,472 MS
```

**File: Entire Large [Calgary Corpus](http://www.data-compression.info/Corpora/CalgaryCorpus/) added to a single uncompressed 7zip archive (miniz.c compression level 9):**
```
E:\lzham\compressiontest\test_compression>test_compression_x64 e:\dev\corpus\calgary_corpus.7z
x64 version
Reading test file 'e:\dev\corpus\calgary_corpus.7z' which is 3,251,898 bytes long.

---------------------------------------------------------------
Testing Compression rate and speed with various compressors.
---------------------------------------------------------------
Compress:CT_STB     :FROM:  3,251,898 TO:  1,065,056 67.25%      317 MS
Compress:CT_MINIZ   :FROM:  3,251,898 TO:  1,065,056 67.25%      317 MS
Compress:CT_CRYPTO  :FROM:  3,251,898 TO:  1,067,241 67.18%      240 MS
Compress:CT_MINILZO :FROM:  3,251,898 TO:  1,581,684 51.36%       25 MS
Compress:CT_ZLIB    :FROM:  3,251,898 TO:  1,059,243 67.43%      351 MS
Compress:CT_BZIP    :FROM:  3,251,898 TO:    961,353 70.44%      273 MS
Compress:CT_LIBLZF  :FROM:  3,251,898 TO:  1,593,245 51.01%       20 MS
Compress:CT_LZMA    :FROM:  3,251,898 TO:    853,241 73.76%    1,267 MS
Compress:CT_FASTLZ  :FROM:  3,251,898 TO:  1,606,374 50.60%       22 MS
Compress:CT_LZHAM   :FROM:  3,251,898 TO:    891,866 72.57%      998 MS

---------------------------------------------------------------
Testing Decompression speed with various decompressors.
---------------------------------------------------------------
Decompress:CT_STB     :FROM:  1,065,056 TO:  3,251,898       22 MS
Decompress:CT_MINIZ   :FROM:  1,065,056 TO:  3,251,898       16 MS
Decompress:CT_CRYPTO  :FROM:  1,067,241 TO:  3,251,898       54 MS
Decompress:CT_MINILZO :FROM:  1,581,684 TO:  3,251,898       13 MS
Decompress:CT_ZLIB    :FROM:  1,059,243 TO:  3,251,898       17 MS
Decompress:CT_BZIP    :FROM:    961,353 TO:  3,251,898      104 MS
Decompress:CT_LIBLZF  :FROM:  1,593,245 TO:  3,251,898       12 MS
Decompress:CT_LZMA    :FROM:    853,241 TO:  3,251,898       54 MS
Decompress:CT_FASTLZ  :FROM:  1,606,374 TO:  3,251,898       13 MS
Decompress:CT_LZHAM   :FROM:    891,866 TO:  3,251,898       38 MS
```

**File: "pic" from the Large [Calgary Corpus](http://www.data-compression.info/Corpora/CalgaryCorpus/) (miniz.c compression level 9):**
```
E:\lzham\compressiontest\test_compression>test_compression_x64 e:\dev\corpus\large_calgary_corpus\pic
x64 version
Reading test file 'e:\dev\corpus\large_calgary_corpus\pic' which is 513,216 bytes long.

---------------------------------------------------------------
Testing Compression rate and speed with various compressors.
---------------------------------------------------------------
Compress:CT_STB     :FROM:    513,216 TO:     53,267 89.62%       65 MS
Compress:CT_MINIZ   :FROM:    513,216 TO:     53,267 89.62%       65 MS
Compress:CT_CRYPTO  :FROM:    513,216 TO:     57,166 88.86%       19 MS
Compress:CT_MINILZO :FROM:    513,216 TO:     86,304 83.18%        2 MS
Compress:CT_ZLIB    :FROM:    513,216 TO:     52,237 89.82%       97 MS
Compress:CT_BZIP    :FROM:    513,216 TO:     49,928 90.27%       16 MS
Compress:CT_LIBLZF  :FROM:    513,216 TO:     80,772 84.26%        1 MS
Compress:CT_LZMA    :FROM:    513,216 TO:     41,897 91.84%      163 MS
Compress:CT_FASTLZ  :FROM:    513,216 TO:     81,960 84.03%        1 MS
Compress:CT_LZHAM   :FROM:    513,216 TO:     47,438 90.76%      506 MS

---------------------------------------------------------------
Testing Decompression speed with various decompressors.
---------------------------------------------------------------
Decompress:CT_STB     :FROM:     53,267 TO:    513,216        2 MS
Decompress:CT_MINIZ   :FROM:     53,267 TO:    513,216        1 MS
Decompress:CT_CRYPTO  :FROM:     57,166 TO:    513,216        5 MS
Decompress:CT_MINILZO :FROM:     86,304 TO:    513,216        1 MS
Decompress:CT_ZLIB    :FROM:     52,237 TO:    513,216        2 MS
Decompress:CT_BZIP    :FROM:     49,928 TO:    513,216        6 MS
Decompress:CT_LIBLZF  :FROM:     80,772 TO:    513,216        1 MS
Decompress:CT_LZMA    :FROM:     41,897 TO:    513,216        4 MS
Decompress:CT_FASTLZ  :FROM:     81,960 TO:    513,216        1 MS
Decompress:CT_LZHAM   :FROM:     47,438 TO:    513,216        7 MS
```

**Files: Uncompressed 7zip archive of an Adobe Acrobat Reader 9.0 installation (miniz.c compression level 9):**
```
E:\lzham\compressiontest\test_compression>test_compression_x64 e:\dev\corpus\adobe.7z
x64 version
Reading test file 'e:\dev\corpus\adobe.7z' which is 146,332,683 bytes long.

---------------------------------------------------------------
Testing Compression rate and speed with various compressors.
---------------------------------------------------------------
Compress:CT_STB     :FROM:146,332,683 TO: 89,209,529 39.04%   12,681 MS
Compress:CT_MINIZ   :FROM:146,332,683 TO: 89,209,529 39.04%   12,681 MS
Compress:CT_CRYPTO  :FROM:146,332,683 TO: 91,494,418 37.48%    7,077 MS
Compress:CT_MINILZO :FROM:146,332,683 TO:101,687,569 30.51%    1,711 MS
Compress:CT_ZLIB    :FROM:146,332,683 TO: 89,166,466 39.07%   13,033 MS
Compress:CT_BZIP    :FROM:146,332,683 TO: 89,072,636 39.13%   18,490 MS
Compress:CT_LIBLZF  :FROM:146,332,683 TO:106,352,717 27.32%    1,070 MS
Compress:CT_LZMA    :FROM:146,332,683 TO: 76,305,505 47.85%   55,814 MS
Compress:CT_FASTLZ  :FROM:146,332,683 TO:105,548,437 27.87%    1,032 MS
Compress:CT_LZHAM   :FROM:146,332,683 TO: 77,260,885 47.20%   37,946 MS

---------------------------------------------------------------
Testing Decompression speed with various decompressors.
---------------------------------------------------------------
Decompress:CT_STB     :FROM: 89,209,529 TO:146,332,683    1,114 MS
Decompress:CT_MINIZ   :FROM: 89,209,529 TO:146,332,683      771 MS
Decompress:CT_CRYPTO  :FROM: 91,494,418 TO:146,332,683    2,133 MS
Decompress:CT_MINILZO :FROM:101,687,569 TO:146,332,683      544 MS
Decompress:CT_ZLIB    :FROM: 89,166,466 TO:146,332,683      816 MS
Decompress:CT_BZIP    :FROM: 89,072,636 TO:146,332,683    7,108 MS
Decompress:CT_LIBLZF  :FROM:106,352,717 TO:146,332,683      562 MS
Decompress:CT_LZMA    :FROM: 76,305,505 TO:146,332,683    5,383 MS
Decompress:CT_FASTLZ  :FROM:105,548,437 TO:146,332,683      556 MS
Decompress:CT_LZHAM   :FROM: 77,260,885 TO:146,332,683    1,734 MS
```

**File: miniz3\_v109.zip (a test of how well various codecs handle already compressed data) (miniz.c compression level 9):**
```
E:\lzham\compressiontest\test_compression>test_compression_x64 e:\dev\miniz3_v109.zip
x64 version
Reading test file 'e:\dev\miniz3_v109.zip' which is 417,549 bytes long.

---------------------------------------------------------------
Testing Compression rate and speed with various compressors.
---------------------------------------------------------------
Compress:CT_STB     :FROM:    417,549 TO:    416,325 0.29%      149 MS
Compress:CT_MINIZ   :FROM:    417,549 TO:    416,325 0.29%      149 MS
Compress:CT_CRYPTO  :FROM:    417,549 TO:    417,658 -0.03%        9 MS
Compress:CT_MINILZO :FROM:    417,549 TO:    417,760 -0.05%        7 MS
Compress:CT_ZLIB    :FROM:    417,549 TO:    416,408 0.27%       18 MS
Compress:CT_BZIP    :FROM:    417,549 TO:    420,125 -0.62%       72 MS
Compress:CT_LIBLZF  :FROM:    417,549 TO:    429,060 -2.76%        4 MS
Compress:CT_LZMA    :FROM:    417,549 TO:    420,196 -0.63%      107 MS
Compress:CT_FASTLZ  :FROM:    417,549 TO:    429,044 -2.75%        3 MS
Compress:CT_LZHAM   :FROM:    417,549 TO:    417,575 -0.01%       77 MS

---------------------------------------------------------------
Testing Decompression speed with various decompressors.
---------------------------------------------------------------
Decompress:CT_STB     :FROM:    416,325 TO:    417,549        1 MS
Decompress:CT_MINIZ   :FROM:    416,325 TO:    417,549        2 MS
Decompress:CT_CRYPTO  :FROM:    417,658 TO:    417,549        2 MS
Decompress:CT_MINILZO :FROM:    417,760 TO:    417,549        2 MS
Decompress:CT_ZLIB    :FROM:    416,408 TO:    417,549        1 MS
Decompress:CT_BZIP    :FROM:    420,125 TO:    417,549       28 MS
Decompress:CT_LIBLZF  :FROM:    429,060 TO:    417,549        2 MS
Decompress:CT_LZMA    :FROM:    420,196 TO:    417,549       29 MS
Decompress:CT_FASTLZ  :FROM:    429,044 TO:    417,549        1 MS
Decompress:CT_LZHAM   :FROM:    417,575 TO:    417,549        2 MS
```