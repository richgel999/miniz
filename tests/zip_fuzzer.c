#include <stdint.h>
#include <inttypes.h>

#include "miniz.h"

static char filename[260];
static unsigned char read_buf[1024 * 256];

static const size_t filename_max = sizeof(filename);
static const size_t read_buf_size = sizeof(read_buf);
static const size_t data_max = 1024 * 256;

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    if (size > data_max)
        return 0;

    int ret = 0;
    mz_zip_archive zip;
    mz_zip_zero_struct(&zip);

    mz_uint flags = 0;

    if (!mz_zip_reader_init_mem(&zip, data, size, flags))
        return 0;

    mz_uint i, files;

    files = mz_zip_reader_get_num_files(&zip);

    for (i = 0; i < files; i++)
    {
        mz_zip_clear_last_error(&zip);

        if (mz_zip_reader_is_file_a_directory(&zip, i))
            continue;

        mz_zip_validate_file(&zip, i, MZ_ZIP_FLAG_VALIDATE_HEADERS_ONLY);

        if (mz_zip_reader_is_file_encrypted(&zip, i))
            continue;

        mz_zip_clear_last_error(&zip);

        mz_uint ret = mz_zip_reader_get_filename(&zip, i, filename, filename_max);

        if (mz_zip_get_last_error(&zip))
            continue;

        mz_zip_archive_file_stat file_stat = { 0 };
        mz_bool status = mz_zip_reader_file_stat(&zip, i, &file_stat) != 0;

        if ((file_stat.m_method) && (file_stat.m_method != MZ_DEFLATED))
            continue;

        mz_zip_reader_extract_file_to_mem(&zip, file_stat.m_filename, read_buf, read_buf_size, 0);
    }

cleanup:
    mz_zip_reader_end(&zip);

    return ret;
}