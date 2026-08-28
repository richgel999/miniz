## Miniz

Miniz is a lossless, high performance data compression library in a single source file that implements the zlib (RFC 1950) and Deflate (RFC 1951) compressed data format specification standards. The support for 32-bit CRC and ISIZE has been added recently and is still under testing.

Miniz  supports the most commonly used functions exported by the zlib library, but is a completely independent implementation so zlib's licensing requirements do not apply. It also contains simple to use functions for writing .PNG format image files and reading/writing/appending .ZIP format archives.

Miniz's compression speed has been tuned to be comparable to zlib's, and it also has a specialized real-time compressor function designed to compare well against fastlz/minilzo.

> [!WARNING]
> 
> The RFC-1952 support has been recently added but is not self-sufficient, yet. It requires that each GZIP stream is provided one after another because miniz isn't able to autonomously inflate the whole stream as-is.

This is limitation can impair theRFC-1952 miniz adoption by end-user, certantly. However, find the end of a GZIP stream is relatively simple also when reading by STDIN.

```c
strm.avail_in = r;
r -= 2; // to avoid the buffer overflow
for (size_t n = 1; n < r; n++) {
    if (inbuf[n  ] == 0x1f
    &&  inbuf[n+1] == 0x8b
    &&  inbuf[n+2] == 0x08
    ){
        strm.avail_in = n;
        rmn = r - n + 2;
        set = n;
        break;
    }
}
```

The variables `rmn` and `set` can be useful for supporting the reading function in appending data because reading char by char would be unsustainable slow and reading everything in memory at once isn't possible in the most general scenario in which a `.gz` file can be arbitrarily long.

## Usage

Releases are available at the [releases page](https://github.com/richgel999/miniz/releases) as a pair of `miniz.c`/`miniz.h` files which can be simply added to a project. To create this file pair the different source and header files are [amalgamated](https://www.sqlite.org/amalgamation.html) during build. Alternatively use as cmake or meson module (or build system of your choice).

```sh
cd minz
SKIPTESTS=1 sh amalgamate.sh
ls -al amalgamation/miniz.[hc]
```

## Features

* MIT licensed
* A portable, single source and header file library written in plain C. Tested with GCC, clang and Visual Studio.
* Easily tuned and trimmed down by defines
* A drop-in replacement for zlib's most used API's (tested in several open source projects that use zlib, such as libpng and libzip).
* Fills a single threaded performance vs. compression ratio gap between several popular real-time compressors and zlib. For example, at level 1, miniz.c compresses around 5-9% better than minilzo, but is approx. 35% slower. At levels 2-9, miniz.c is designed to compare favorably against zlib's ratio and speed. See the miniz performance comparison page for example timings.
* Not a block based compressor: miniz.c fully supports stream based processing using a coroutine-style implementation. The zlib-style API functions can be called a single byte at a time if that's all you've got.
* Easy to use. The low-level compressor (tdefl) and decompressor (tinfl) have simple state structs which can be saved/restored as needed with simple memcpy's. The low-level codec API's don't use the heap in any way.
* Entire inflater (including optional zlib header parsing and Adler-32 checking) is implemented in a single function as a coroutine, which is separately available in a small (~550 line) source file: miniz_tinfl.c
* A fairly complete (but totally optional) set of .ZIP archive manipulation and extraction API's. The archive functionality is intended to solve common problems encountered in embedded, mobile, or game development situations. (The archive API's are purposely just powerful enough to write an entire archiver given a bit of additional higher-level logic.)

## Building miniz - Using vcpkg

You can download and install miniz using the [vcpkg](https://github.com/Microsoft/vcpkg) dependency manager:

    git clone https://github.com/Microsoft/vcpkg.git
    cd vcpkg
    ./bootstrap-vcpkg.sh
    ./vcpkg integrate install
    ./vcpkg install miniz

The miniz port in vcpkg is kept up to date by Microsoft team members and community contributors. If the version is out of date, please [create an issue or pull request](https://github.com/Microsoft/vcpkg) on the vcpkg repository.

## Known Problems

* No support for encrypted archives. Not sure how useful this stuff is in practice.
* Minimal documentation. The assumption is that the user is already familiar with the basic zlib API. I need to write an API wiki - for now I've tried to place key comments before each enum/API, and I've included 6 examples that demonstrate how to use the module's major features.

## Special Thanks

Thanks to Alex Evans for the PNG writer function. Also, thanks to Paul Holden and Thorsten Scheuermann for feedback and testing, Matt Pritchard for all his encouragement, and Sean Barrett's various public domain libraries for inspiration (and encouraging me to write miniz.c in C, which was much more enjoyable and less painful than I thought it would be considering I've been programming in C++ for so long).

Thanks to Bruce Dawson for reporting a problem with the level_and_flags archive API parameter (which is fixed in v1.12) and general feedback, and Janez Zemva for indirectly encouraging me into writing more examples.

## Patents

I was recently asked if miniz avoids patent issues. miniz purposely uses the same core algorithms as the ones used by zlib. The compressor uses vanilla hash chaining as described [here](https://datatracker.ietf.org/doc/html/rfc1951#section-4). Also see the [gzip FAQ](https://web.archive.org/web/20160308045258/http://www.gzip.org/#faq11). In my opinion, if miniz falls prey to a patent attack then zlib/gzip are likely to be at serious risk too.
