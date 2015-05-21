[miniz.c](http://code.google.com/p/miniz/source/browse/trunk/miniz.c) is a lossless, high performance data compression library in a single source file that implements the [zlib](http://en.wikipedia.org/wiki/Zlib) ([RFC 1950](http://www.ietf.org/rfc/rfc1950.txt)) and [Deflate](http://en.wikipedia.org/wiki/Deflate) ([RFC 1951](http://www.ietf.org/rfc/rfc1951.txt)) compressed data format specification standards. It supports the most commonly used functions exported by the [zlib library](http://www.zlib.net/), but is a completely independent implementation so zlib's licensing requirements do not apply. miniz.c also contains simple to use functions for writing [.PNG format](http://en.wikipedia.org/wiki/Portable_Network_Graphics) image files and reading/writing/appending [.ZIP format](http://en.wikipedia.org/wiki/ZIP_(file_format)) archives. miniz's compression speed has been tuned to be comparable to zlib's, and it also has a specialized real-time compressor function designed to compare well against fastlz/minilzo.

Here's a [blog update](http://richg42.blogspot.com/2013/10/minizc-finally-added-zip64-support.html) on miniz bug fixes/enhancements, and the current status of zip64 support. The source to the zip64 variant of miniz was released as part of Valve's vogl codebase:
http://richg42.blogspot.com/2014/03/zip64-version-of-miniz-library-released.html

Whenever I get the chance I'll be extracting it and releasing it separately.

### Features ###
  * Completely free: Public domain in jurisdictions that recognize copyright laws, with a license patterned after the public domain SQLite project, see [unlicense.org](http://unlicense.org/).
  * A portable, single source file header file library written in plain C. I've tested with clang v3.3, various versions of gcc, mingw, MSVC 2008/2010, and TCC (Tiny C Compiler) v0.9.26, under both Linux and Windows x86/x64. Earlier versions of miniz where also tested on OSX, and miniz.c has shipped in several iOS games.
  * Easily tuned and trimmed down by configuring macros at the top of the source file.
  * A drop-in replacement for zlib's most used API's (tested in several open source projects that use zlib, such as libpng and libzip).
  * Fills a single threaded performance vs. compression ratio gap between several popular real-time compressors and zlib. For example, at level 1, miniz.c compresses around 5-9% better than [minilzo](http://oldhome.schmorp.de/marc/liblzf.html), but is approx. 35% slower. At levels 2-9, miniz.c is designed to compare favorably against zlib's ratio and speed. See the [miniz performance comparison](http://code.google.com/p/miniz/wiki/miniz_performance_comparison_v110) page for example timings.
  * Not a block based compressor: miniz.c fully supports stream based processing using a coroutine-style implementation. The zlib-style API functions can be called a single byte at a time if that's all you've got.
  * Easy to use. The low-level compressor (tinfl) and decompressor (tdefl) have simple state structs which can be saved/restored as needed with simple memcpy's. The low-level codec API's don't use the heap in any way.
  * Entire inflater (including optional zlib header parsing and Adler-32 checking) is implemented in a single function as a coroutine, which is separately available in a small (~550 line) source file: [tinfl.c](http://code.google.com/p/miniz/source/browse/trunk/tinfl.c)
  * A fairly complete (but totally optional) set of .ZIP archive manipulation and extraction API's. The archive functionality is intended to solve common problems encountered in embedded, mobile, or game development situations. (The archive API's are purposely just powerful enough to write an entire archiver given a bit of additional higher-level logic.)

I've created a [wiki page](http://code.google.com/p/miniz/wiki/miniz_performance_comparison_v110) comparing the speed and compression ratio of miniz.c verses various other open source codecs. miniz.c's compression ratio is very close to zlib's (sometimes better, or sometimes a small amount worse because it tends to output more blocks due to using less memory), and is typically (but not always) faster, even without any platform specific assembly language optimizations.

### Release History ###
  * v1.16 BETA - Oct 19, 2013: Still testing, this release is downloadable from [here](http://www.tenacioussoftware.com/miniz_v116_beta_r1.7z). Two key inflator-only robustness and streaming related changes. Also merged in tdefl\_compressor\_alloc(), tdefl\_compressor\_free() helpers to make script bindings easier for rustyzip. I would greatly appreciate any help with testing or any feedback.

> The inflator in raw (non-zlib) mode is now usable on gzip or similar streams that have a bunch of bytes following the raw deflate data (problem discovered by rustyzip author williamw520). This version should **never** read beyond the last byte of the raw deflate data independent of how many bytes you pass into the input buffer.

> The inflator now has a new failure status TINFL\_STATUS\_FAILED\_CANNOT\_MAKE\_PROGRESS (-4). Previously, if the inflator was starved of bytes and could not make progress (because the input buffer was empty and the caller did not set the TINFL\_FLAG\_HAS\_MORE\_INPUT flag - say on truncated or corrupted compressed data stream) it would append all 0's to the input and try to soldier on. This is scary behavior if the caller didn't know when to stop accepting output (because it didn't know how much uncompressed data was expected, or didn't enforce a sane maximum). v1.16 will instead return TINFL\_STATUS\_FAILED\_CANNOT\_MAKE\_PROGRESS immediately if it needs 1 or more bytes to make progress, the input buf is empty, and the caller has indicated that no more input is available. This is a "soft" failure, so you can call the inflator again with more input and it will try to continue, or you can give up and fail. This could be very useful in network streaming scenarios.

> The other minor changes are documented at the top of miniz.c.

  * v1.15 `r4` STABLE - Oct 13, 2013: Merged over a few very minor bug fixes that I fixed in the zip64 branch. This is downloadable from [here](http://code.google.com/p/miniz/downloads/list) and also in SVN head (as of 10/19/13).
  * v1.15 - Oct. 13, 2013: Interim bugfix release while I work on the next major release with zip64 and streaming compression/decompression support. Fixed the MZ\_ZIP\_FLAG\_DO\_NOT\_SORT\_CENTRAL\_DIRECTORY bug (thanks kahmyong.moon@hp.com), which could cause the locate files func to not find files when this flag was specified. Also fixed a bug in mz\_zip\_reader\_extract\_to\_mem\_no\_alloc() with user provided read buffers (thanks kymoon). I also merged lots of compiler fixes from various github repo branches and Google Code issue reports. I finally added cmake support (only tested under for Linux so far), compiled and tested with clang v3.3 and gcc 4.6 (under Linux), added defl\_write\_image\_to\_png\_file\_in\_memory\_ex() (supports Y flipping for OpenGL use, real-time compression), added a new PNG example (example6.c - Mandelbrot), and I added 64-bit file I/O support (stat64(), etc.) for glibc.
  * v1.14 - May 20, 2012: (SVN Only) Minor tweaks to get miniz.c compiling with the [Tiny C Compiler](http://bellard.org/tcc), added `#ifndef MINIZ_NO_TIME` guards around `utime.h` includes. Adding `mz_free()` function, so the caller can free heap blocks returned by miniz using whatever heap functions it has been configured to use, MSVC specific fixes to use "safe" variants of several functions (localtime\_s, fopen\_s, freopen\_s).
  * v1.14 - May 20, 2012: Compiler specific fixes, some from fermtect. I upgraded to TDM GCC 4.6.1 and now `static __forceinline` is giving it fits, so I'm changing all usage of `__forceinline` to MZ\_FORCEINLINE and forcing gcc to use `__attribute__((__always_inline__))` (and MSVC to use `__forceinline`). Also various fixes from fermtect for MinGW32: added #include <time.h>, 64-bit ftell/fseek fixes.
  * v1.13 - May 19, 2012: From jason@cornsyrup.org and kelwert@mtu.edu - Most importantly, fixed mz\_crc32() so it doesn't compute the wrong CRC-32's when mz\_ulong is 64-bits. Temporarily/locally slammed in "typedef unsigned long mz\_ulong" and re-ran a randomized regression test on ~500k files. Other stuff:

> Eliminated a bunch of warnings when compiling with GCC 32-bit/64. Ran all examples, miniz.c, and tinfl.c through MSVC 2008's /analyze (static analysis) option and fixed all warnings (except for the silly "Use of the comma-operator in a tested expression.." analysis warning, which I purposely use to work around a MSVC compiler warning).

> Created 32-bit and 64-bit Codeblocks projects/workspace. Built and tested Linux executables. The codeblocks workspace is compatible with Linux+Win32/x64. Added miniz\_tester solution/project, which is a useful little app derived from LZHAM's tester app that I use as part of the regression test. Ran miniz.c and tinfl.c through another series of regression testing on ~500,000 files and archives. Modified example5.c so it purposely disables a bunch of high-level functionality (MINIZ\_NO\_STDIO, etc.). (Thanks to corysama for the MINIZ\_NO\_STDIO bug report.)

> Fix ftell() usage in a few of the examples so they exit with an error on files which are too large (a limitation of the examples, not miniz itself). Fix fail logic handling in mz\_zip\_add\_mem\_to\_archive\_file\_in\_place() so it always calls mz\_zip\_writer\_finalize\_archive() and mz\_zip\_writer\_end(), even if the file add fails.

### Details ###

miniz.c employs several proven techniques and approaches used in my much more powerful (but slower and more complex) lossless [lzham](http://code.google.com/p/lzham/) codec, and in my [jpeg-compressor](http://code.google.com/p/jpeg-compressor/) project. Specifically, miniz.c's linear-time Huffman codelength generator, the single switch-statement approach used to implement the decompressor as a single function [coroutine](http://en.wikipedia.org/wiki/Coroutine), and its very fast [Huffman](http://en.wikipedia.org/wiki/Huffman_coding) code symbol unpacker are all loosely based off the implementations I wrote for these earlier projects.

miniz.c v1.10 includes an optimized real-time compressor written specifically for compression level 1 (MZ\_BEST\_SPEED). miniz.c's level 1 compression ratio is around 5-9% higher than other real-time compressors, such as [minilzo](http://www.oberhumer.com/opensource/lzo/), [fastlz](http://www.fastlz.org/), or [liblzf](http://oldhome.schmorp.de/marc/liblzf.html). miniz.c's level 1 data consumption rate on a Core i7 3.2 GHz typically ranges between 70-120.5 MB/sec. Between levels 2-9, miniz.c is designed to compare favorably against zlib, where it typically has roughly equal or better performance.

miniz.c has been compiled and tested under Windows (x86 and x64, using MSVC 2005/2008 and GCC 4.5.0 using TDM-GCC x64), and under 32-bit Ubuntu Linux (Lucid Lynx using gcc). The code should be endian safe, but I have not tested it on a big endian platform yet. For maximum compatibility, unused or unnecessary features of miniz.c can be completely disabled via #define's - see the code for details.

miniz's PNG writer has been verified using the [pngcheck](http://www.libpng.org/pub/png/apps/pngcheck.html) tool.

If you use miniz, I would greatly appreciate it if you sent me an email with any feedback, or info on how you're using it in practice.

### Implemented zlib Functions ###

miniz.c implements the following standard zlib functions:

  * Compression: `deflateInit`, `deflateInit2`, `deflateReset`, `deflate`, `deflateEnd`, `deflateBound`
  * Single call compression: `compress`, `compress2`, `compressBound`
  * Decompression: `inflateInit`, `inflateInit2`, `inflate`, `inflateEnd`
  * Single call decompression: `uncompress`
  * Allocation callbacks: `alloc_func`, `free_func`
  * Stream structure: `z_stream`
  * Misc. functions: `crc32`, `adler32`, `zError`, `zlib_version`

Note, miniz.c actually prefixes its zlib-like functions and macros with the `mz_` or `MZ_` prefixes, and it uses a set of macros to redefine the zlib symbols to miniz's internal symbols. These optional remapping macros can be disabled by defining the `MINIZ_NO_ZLIB_COMPATIBLE_NAMES` macro, allowing miniz.c to be mixed with zlib in the same compilation unit.

### Simple Examples ###

Include [miniz.c](http://code.google.com/p/miniz/source/browse/trunk/miniz.c) and call one of these (zlib-style) helper functions for:

Memory to Memory Compression

```
  // Returns Z_OK on success, or one of the error codes from deflate() on failure.
  int compress(Byte *pDest, uLong *pDest_len, const Byte *pSource, uLong source_len);

  // Like compress() but with more control, level may range from 0 (storing) to 9 (max. compression)
  int compress2(Byte *pDest, uLong *pDest_len, const Byte *pSource, uLong source_len, int level);
```

Memory to Memory Decompression
```
  // Returns Z_OK on success, or one of the error codes from inflate() on failure.
  int uncompress(Byte *pDest, uLong *pDest_len, const Byte *pSource, uLong source_len);
```

Extract a single file from a ZIP archive:
```
  // Returns pointer to extracted file, or NULL on failure.
  void *mz_zip_extract_archive_file_to_heap(const char *pZip_filename, const char *pArchive_name, 
    size_t *pSize, mz_uint zip_flags);
```

Add a single file to an existing ZIP archive, or create a new archive, with optional file comment:
```
  mz_bool mz_zip_add_mem_to_archive_file_in_place(const char *pZip_filename, const char *pArchive_name,
    const void *pBuf, size_t buf_size, const void *pComment, mz_uint16 comment_size, mz_uint level_and_flags);
```

Write a .PNG format image to a file in memory:
```
  void *tdefl_write_image_to_png_file_in_memory(const void *pImage, int w, int h, int num_chans, size_t *pLen_out);
```

These are just basic examples. For more control, miniz.c has many helper functions to compress zlib and raw deflate streams to/from files, memory, the heap, or user supplied callbacks. For maximum control, miniz.c also includes a set of low-level "tinfl" (decompression) and "tdefl" (compression) coroutine functions that read and write to user supplied buffers and do not use the heap in any way -- see the top of the file for more details. (Internally, all high level compression related functions are implemented in terms of tinfl and tdefl.)

### Known Problems ###
  * No support for encrypted or zip64 archives. I'm not sure how useful this stuff is in practice. Zip encryption is very weak, and it's unclear how many people are interested in reading/writing huge archives with a small library like this. Please email if you're interested in these features. Oct. 13, 2013: OK, I've implemented zip64 which I needed for a project at work. It's **a lot** of new code, so much that I'm going to split up miniz.c into two files (miniz.c for the codec bits, and miniz\_zip.c for archive handling). I'm not going to release it until I get to bang on it more, but email me if you would like to take a peek.
  * Minimal documentation. I'm assuming the user is already familiar with the basic zlib API. I need to write an API wiki - for now I've tried to place key comments before each enum/API, and I've included 6 examples that demonstrate how to use the module's major features.

## Special Thanks ##
Thanks to Alex Evans for the PNG writer function. Also, thanks to Paul Holden and Thorsten Scheuermann for feedback and testing, Matt Pritchard for all his encouragement, and [Sean Barrett](http://nothings.org/)'s various public domain libraries for inspiration (and encouraging me to write miniz.c in C, which was much more enjoyable and less painful than I thought it would be considering I've been programming in C++ for so long).

Thanks to Bruce Dawson <bruced@valvesoftware.com> for reporting a problem with the level\_and\_flags archive API parameter (which is fixed in v1.12) and general feedback, and Janez Zemva <janezz55@gmail.com> for indirectly encouraging me into writing more examples.

## Patents ##
I was recently asked if miniz avoids patent issues. miniz purposely uses the same core algorithms as the ones used by zlib. The compressor uses vanilla hash chaining as described [here](http://www.gzip.org/zlib/rfc-deflate.html#algorithm). Also see the [gzip FAQ](http://www.gzip.org/#faq11). In my opinion, if miniz falls prey to a patent attack then zlib/gzip are likely to be at serious risk too.

### Building the Examples Under Linux ###

v1.15 includes a CMakeLists.txt file to use with cmake:

```
   cmake .
   make clean
   make
```

This will build the x64 release examples by default on x64 systems. Optionally, you can use ccmake (package cmake-curses-gui) to configure the project after running cmake ("ccmake ."). I've built and tested v1.15 with tcc 0.9.26 (Tiny C Compiler), clang v3.3, and gcc 4.6.3.

### Example Projects ###
I've included a Visual Studio 2008 solution (examples.sln) with 6 examples:
  * [example1.c](http://code.google.com/p/miniz/source/browse/trunk/example1.c): Demonstrates miniz.c's compress() and uncompress() functions (same as zlib's), also a crude decompressor fuzzy test.
  * [example2.c](http://code.google.com/p/miniz/source/browse/trunk/example2.c): Demonstration of miniz.c's ZIP archive API's. Adds a bunch of files to test.zip, dumps file stat info on each file in the archive, then extracts a single file into memory.
  * [example3.c](http://code.google.com/p/miniz/source/browse/trunk/example3.c): Command line tool for file compression/decompression. Demonstrates how to use miniz.c's deflate() and inflate() functions to handle streaming compression/decompression.
  * [example4.c](http://code.google.com/p/miniz/source/browse/trunk/example4.c): Demonstrates memory to callback decompression of compressed zlib streams (can decompress files generated by example3). Simple demo of [tinfl.c](http://code.google.com/p/miniz/source/browse/trunk/tinfl.c), which is just the Inflate functionality pulled from the larger miniz.c. tinfl.c is a full single-function Inflate implementation with zlib header parsing and Adler32 checking in ~550 lines of code+comments.
  * [example5.c](http://code.google.com/p/miniz/source/browse/trunk/example5.c): Demonstrates how to use the lowest level (non-zlib compatible) API's for file to file compression. The low-level API's are less forgiving, but more flexible, faster, and don't ever touch the heap. The low-level API's are implemented as coroutines, so they can be efficiently used for any sort of streaming scenario.
  * [example6.c](http://code.google.com/p/miniz/source/browse/trunk/example6.c): Simple PNG writing demonstration.
  * As of v1.13 I also include one of the test app (miniz\_tester.cpp) I used to regression test miniz. This command line tool is only intended for testing, and it wasn't intended to be an example, but I'm releasing it in case someone finds it useful.


---


For any questions or problems with this code please contact Rich Geldreich at <richgel99 at gmail.com>. Here's my [twitter page](http://twitter.com/#!/richgel999).