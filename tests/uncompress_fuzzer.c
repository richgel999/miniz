/* Derived from zlib fuzzers at http://github.com/google/oss-fuzz/tree/master/projects/zlib,
 * see ossfuzz.sh for full license text.
*/

#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "miniz.h"

int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    unsigned long int buffer_length = 1;
    unsigned char *buffer = NULL;
    int z_status = 0;

    if (size > 0)
        buffer_length *= data[0];
    if (size > 1)
        buffer_length *= data[1];

    buffer = (unsigned char *)malloc(buffer_length);

    z_status = uncompress(buffer, &buffer_length, data, size);
    free(buffer);

    if (Z_OK != z_status)
        return 0;
    return 0;
}
