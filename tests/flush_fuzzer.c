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

void test_flush(unsigned char *compr, size_t *comprLen)
{
    z_stream c_stream; /* compression stream */
    int err;
    unsigned int len = dataLen;

    c_stream.zalloc = zalloc;
    c_stream.zfree = zfree;
    c_stream.opaque = NULL;

    err = deflateInit(&c_stream, Z_DEFAULT_COMPRESSION);
    CHECK_ERR(err, "deflateInit");

    c_stream.next_in = (Bytef *)data;
    c_stream.next_out = compr;
    c_stream.avail_in = 3;
    c_stream.avail_out = (unsigned int)*comprLen;
    err = deflate(&c_stream, Z_FULL_FLUSH);
    CHECK_ERR(err, "deflate flush 1");

    compr[3]++; /* force an error in first compressed block */
    c_stream.avail_in = len - 3;

    err = deflate(&c_stream, Z_FINISH);

    if (err != Z_STREAM_END)
    {
        CHECK_ERR(err, "deflate flush 2");
    }

    err = deflateEnd(&c_stream);
    CHECK_ERR(err, "deflateEnd");

    *comprLen = (size_t)c_stream.total_out;
}

int LLVMFuzzerTestOneInput(const uint8_t *d, size_t size)
{
    size_t comprLen = 100 + 2 * compressBound(size);
    size_t uncomprLen = size;
    uint8_t *compr, *uncompr;

    /* Discard inputs larger than 1Mb. */
    static const size_t kMaxSize = 1024 * 1024;

    /* This test requires at least 3 bytes of input data. */
    if (size <= 3 || size > kMaxSize)
        return 0;

    data = d;
    dataLen = size;
    compr = calloc(1, comprLen);
    uncompr = calloc(1, uncomprLen);

    test_flush(compr, &comprLen);

    free(compr);
    free(uncompr);

    return 0;
}
