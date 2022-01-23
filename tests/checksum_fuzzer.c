/* Derived from zlib fuzzers at http://github.com/google/oss-fuzz/tree/master/projects/zlib,
 * see ossfuzz.sh for full license text.
*/

#include <stddef.h>
#include <stdint.h>
#include <inttypes.h>

#include "miniz.h"

static const size_t kMaxSize = 1024 * 1024;

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t dataLen)
{
    /* Discard inputs larger than 1Mb. */
    if (dataLen < 1 || dataLen > kMaxSize) return 0;

    uint32_t crc = crc32(0L, NULL, 0);
    uint32_t adler = adler32(0L, NULL, 0);

    crc = crc32(crc, data, (uint32_t) dataLen);
    adler = adler32(adler, data, (uint32_t) dataLen);

    return 0;
}
