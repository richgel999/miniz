#ifdef INCLUDE_MINIZ_SUBDIR
#include <miniz/miniz.h>
#else
#include <miniz.h>
#endif
#include <string.h>

int main(void)
{
    const unsigned char input[] = "miniz installed headers";
    unsigned char compressed[128], output[128];
    mz_ulong compressed_size = sizeof(compressed), output_size = sizeof(output);
    if (mz_compress(compressed, &compressed_size, input, sizeof(input)) != MZ_OK)
        return 1;
    if (mz_uncompress(output, &output_size, compressed, compressed_size) != MZ_OK)
        return 2;
    return output_size != sizeof(input) || memcmp(input, output, sizeof(input)) != 0;
}
