/* Derived from zlib fuzzers at http://github.com/google/oss-fuzz/tree/master/projects/zlib,
 * see ossfuzz.sh for full license text.
 */

#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>

#include "miniz.h"

#define CHECK_ERR(err, msg)                              \
    {                                                    \
        if (err != Z_OK)                                 \
        {                                                \
            fprintf(stderr, "%s error: %d\n", msg, err); \
            exit(1);                                     \
        }                                                \
    }

static const uint8_t *data;
static size_t dataLen;
static alloc_func zalloc = NULL;
static free_func zfree = NULL;
static unsigned int diff;

/* Test deflate() with large buffers and dynamic change of compression level */
void test_large_deflate(unsigned char *compr, size_t comprLen,
                        unsigned char *uncompr, size_t uncomprLen)
{
    z_stream c_stream; /* compression stream */
    int err;

    c_stream.zalloc = zalloc;
    c_stream.zfree = zfree;
    c_stream.opaque = NULL;

    err = deflateInit(&c_stream, Z_BEST_COMPRESSION);
    CHECK_ERR(err, "deflateInit");

    c_stream.next_out = compr;
    c_stream.avail_out = (unsigned int)comprLen;

    /* At this point, uncompr is still mostly zeroes, so it should compress
     * very well:
     */
    c_stream.next_in = uncompr;
    c_stream.avail_in = (unsigned int)uncomprLen;
    err = deflate(&c_stream, Z_NO_FLUSH);
    CHECK_ERR(err, "deflate large 1");

    if (c_stream.avail_in != 0)
    {
        fprintf(stderr, "deflate not greedy\n");
        exit(1);
    }

    /* Feed in already compressed data: */
    c_stream.next_in = compr;
    diff = (unsigned int)(c_stream.next_out - compr);
    c_stream.avail_in = diff;

    deflate(&c_stream, Z_NO_FLUSH);
    err = deflate(&c_stream, Z_FINISH);

    if (err != Z_STREAM_END)
    {
        fprintf(stderr, "deflate large should report Z_STREAM_END\n");
        exit(1);
    }
    err = deflateEnd(&c_stream);
    CHECK_ERR(err, "deflateEnd");
}

/* Test inflate() with large buffers */
void test_large_inflate(unsigned char *compr, size_t comprLen,
                        unsigned char *uncompr, size_t uncomprLen)
{
    int err;
    z_stream d_stream; /* decompression stream */

    d_stream.zalloc = zalloc;
    d_stream.zfree = zfree;
    d_stream.opaque = NULL;

    d_stream.next_in = compr;
    d_stream.avail_in = (unsigned int)comprLen;

    err = inflateInit(&d_stream);
    CHECK_ERR(err, "inflateInit");

    for (;;)
    {
        d_stream.next_out = uncompr; /* discard the output */
        d_stream.avail_out = (unsigned int)uncomprLen;
        err = inflate(&d_stream, Z_NO_FLUSH);
        if (err == Z_STREAM_END)
            break;

        CHECK_ERR(err, "large inflate");
    }

    err = inflateEnd(&d_stream);
    CHECK_ERR(err, "inflateEnd");
}

int LLVMFuzzerTestOneInput(const uint8_t *d, size_t size)
{
    size_t comprLen = 100 + 3 * size;
    size_t uncomprLen = comprLen;
    uint8_t *compr, *uncompr;

    /* Discard inputs larger than 512Kb. */
    static size_t kMaxSize = 512 * 1024;

    if (size < 1 || size > kMaxSize)
        return 0;

    data = d;
    dataLen = size;
    compr = calloc(1, comprLen);
    uncompr = calloc(1, uncomprLen);

    test_large_deflate(compr, comprLen, uncompr, uncomprLen);
    test_large_inflate(compr, comprLen, uncompr, uncomprLen);

    free(compr);
    free(uncompr);

    return 0;
}
