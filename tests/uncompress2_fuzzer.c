/* Derived from zlib fuzzers at http://github.com/google/oss-fuzz/tree/master/projects/zlib,
 * see ossfuzz.sh for full license text.
 */

#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "miniz.h"

static unsigned char buffer[256 * 1024] = { 0 };

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    unsigned long int buffer_length = sizeof(buffer);

    if (Z_OK != uncompress2(buffer, &buffer_length, data, &size))
        return 0;

    return 0;
}
