#include "catch_amalgamated.hpp"
#include "../miniz.h"
#include "miniz_zip.h"
#include <assert.h>
#include <string>
#include <vector>

#ifdef _WIN32
#define unlink _unlink
#else
#include <unistd.h>
#endif /* _WIN32 */

#ifndef MINIZ_NO_ARCHIVE_WRITING_APIS
struct short_write_context
{
    bool rejected_data_descriptor = false;
};

static size_t reject_data_descriptor(void *opaque, mz_uint64 file_ofs, const void *buffer, size_t size)
{
    auto *context = static_cast<short_write_context *>(opaque);
    const auto *bytes = static_cast<const mz_uint8 *>(buffer);
    (void)file_ofs;

    if ((size == 16) && (bytes[0] == 0x50) && (bytes[1] == 0x4b) && (bytes[2] == 0x07) && (bytes[3] == 0x08))
    {
        context->rejected_data_descriptor = true;
        return size - 1;
    }

    return size;
}

struct read_context
{
    const mz_uint8 *data;
    size_t size;
};

static size_t read_data(void *opaque, mz_uint64 file_ofs, void *buffer, size_t size)
{
    const auto *context = static_cast<const read_context *>(opaque);
    size_t offset = static_cast<size_t>(file_ofs);
    size_t remaining;

    if (offset >= context->size)
        return 0;

    remaining = context->size - offset;
    if (size > remaining)
        size = remaining;
    memcpy(buffer, context->data + offset, size);
    return size;
}

TEST_CASE("ZIP writer reports data descriptor write failures")
{
    const mz_uint8 contents[] = "contents";

    SECTION("memory input")
    {
        mz_zip_archive zip = {};
        short_write_context write_context;
        zip.m_pWrite = reject_data_descriptor;
        zip.m_pIO_opaque = &write_context;
        REQUIRE(mz_zip_writer_init(&zip, 0));

        mz_bool result = mz_zip_writer_add_mem(&zip, "memory.txt", contents, sizeof(contents) - 1, MZ_DEFAULT_COMPRESSION);
        mz_zip_error error = mz_zip_get_last_error(&zip);
        mz_bool end_result = mz_zip_writer_end(&zip);

        REQUIRE_FALSE(result);
        REQUIRE(write_context.rejected_data_descriptor);
        INFO("last error: " << static_cast<int>(error));
        REQUIRE(error == MZ_ZIP_FILE_WRITE_FAILED);
        REQUIRE(end_result);
    }

    SECTION("read callback input")
    {
        mz_zip_archive zip = {};
        short_write_context write_context;
        read_context input = {contents, sizeof(contents) - 1};
        zip.m_pWrite = reject_data_descriptor;
        zip.m_pIO_opaque = &write_context;
        REQUIRE(mz_zip_writer_init(&zip, 0));

        mz_bool result = mz_zip_writer_add_read_buf_callback(&zip, "callback.txt", read_data, &input, input.size,
                                                              nullptr, nullptr, 0, MZ_DEFAULT_COMPRESSION,
                                                              nullptr, 0, nullptr, 0);
        mz_zip_error error = mz_zip_get_last_error(&zip);
        mz_bool end_result = mz_zip_writer_end(&zip);

        REQUIRE_FALSE(result);
        REQUIRE(write_context.rejected_data_descriptor);
        INFO("last error: " << static_cast<int>(error));
        REQUIRE(error == MZ_ZIP_FILE_WRITE_FAILED);
        REQUIRE(end_result);
    }
}
#endif

#ifndef MINIZ_NO_STDIO
bool create_test_zip(const bool zip64)
{
    unlink("test.zip");
    mz_zip_archive zip_archive = {};
    auto b = mz_zip_writer_init_file_v2(&zip_archive, "test.zip", 0, zip64 ? MZ_ZIP_FLAG_WRITE_ZIP64 : 0);
    if (!b)
        return false;

    b = mz_zip_writer_add_mem(&zip_archive, "test.txt", "foo", 3, MZ_DEFAULT_COMPRESSION);
    if (!b)
        return false;

    b = mz_zip_writer_finalize_archive(&zip_archive);
    if (!b)
        return false;

    b = mz_zip_writer_end(&zip_archive);
    if (!b)
        return false;

    return true;
}

TEST_CASE("Zip writer tests")
{
    auto b = create_test_zip(false);
    REQUIRE(b);

    SECTION("Test test.txt content correct")
    {
        mz_zip_archive zip_archive = {};

        auto b = mz_zip_reader_init_file(&zip_archive, "test.zip", 0);
        REQUIRE(b);

        size_t content_size;
        auto content = mz_zip_reader_extract_file_to_heap(&zip_archive, "test.txt", &content_size, 0);

        std::string_view content_view(reinterpret_cast<char *>(content), content_size);

        REQUIRE(content_view == "foo");
        REQUIRE(content_view.size() == 3);

        free(content);

        mz_zip_reader_end(&zip_archive);
    }

    SECTION("Test repeated file addition to zip")
    {
        mz_zip_archive zip_archive = {};
        auto b = mz_zip_writer_init_file(&zip_archive, "test2.zip", 0);
        REQUIRE(b);

        b = mz_zip_writer_finalize_archive(&zip_archive);
        REQUIRE(b);

        b = mz_zip_writer_end(&zip_archive);
        REQUIRE(b);

        for (int i = 0; i < 50; i++)
        {
            const char *str = "hello world";
            b = mz_zip_add_mem_to_archive_file_in_place(
                std::string("test2.zip").c_str(), ("file1.txt" + std::to_string(i)).c_str(),
                str, (mz_uint16)strlen(str),
                NULL, 0,
                MZ_BEST_COMPRESSION);
            REQUIRE(b);
        }
    }
}

TEST_CASE("Zip reader tests")
{
    const auto b = create_test_zip(true);
    REQUIRE(b);

    SECTION("Test zip file reading")
    {
        mz_zip_archive zip_archive = {};

        auto b = mz_zip_reader_init_file(&zip_archive, "test.zip", 0);
        REQUIRE(b);

        size_t num_files = mz_zip_reader_get_num_files(&zip_archive);
        REQUIRE(num_files == 1);

        mz_zip_archive_file_stat file_stat;
        b = mz_zip_reader_file_stat(&zip_archive, 0, &file_stat);
        REQUIRE(b);

        REQUIRE(file_stat.m_file_index == 0);
        REQUIRE(file_stat.m_uncomp_size == 3);
        REQUIRE(file_stat.m_comp_size == 3);
        REQUIRE(std::string_view(file_stat.m_filename) == "test.txt");

        mz_zip_reader_end(&zip_archive);
    }

    SECTION("Test central dir overflow")
    {
        auto f = fopen("test.zip", "rb");
        REQUIRE(f);
        char buf[1000];
        const auto read = fread(buf, 1, 1000, f);
        fclose(f);

        unsigned long long cdir_ofs = -1;
        memcpy(buf + 159, &cdir_ofs, sizeof(cdir_ofs));

        unlink("test.zip");
        f = fopen("test.zip", "wb");
        REQUIRE(f);
        fwrite(buf, 1, read, f);
        fclose(f);

        mz_zip_archive zip_archive = {};

        auto b = mz_zip_reader_init_file(&zip_archive, "test.zip", 0);
        REQUIRE(!b);
        REQUIRE(zip_archive.m_last_error == MZ_ZIP_INVALID_HEADER_OR_CORRUPTED);
    }

    SECTION("Test malformed archive scan does not underflow")
    {
        std::vector<unsigned char> data(4097, 'A');
        mz_zip_archive zip_archive = {};

        auto b = mz_zip_reader_init_mem(&zip_archive, data.data(), data.size(), 0);
        REQUIRE(!b);
        REQUIRE(zip_archive.m_last_error == MZ_ZIP_FAILED_FINDING_CENTRAL_DIR);
    }
}

#endif /* MINIZ_NO_STDIO */

TEST_CASE("Tinfl / tdefl tests")
{
    SECTION("simple_test1")
    {
        size_t cmp_len = 0;

        const char *p = "This is a test.This is a test.This is a test.1234567This is a test.This is a test.123456";
        size_t uncomp_len = strlen(p);

        void *pComp_data = tdefl_compress_mem_to_heap(p, uncomp_len, &cmp_len, TDEFL_WRITE_ZLIB_HEADER);
        REQUIRE(pComp_data);

        size_t decomp_len = 0;
        void *pDecomp_data = tinfl_decompress_mem_to_heap(pComp_data, cmp_len, &decomp_len, TINFL_FLAG_PARSE_ZLIB_HEADER);

        REQUIRE(pDecomp_data);
        REQUIRE(decomp_len == uncomp_len);
        REQUIRE(memcmp(pDecomp_data, p, uncomp_len) == 0);

        free(pComp_data);
        free(pDecomp_data);
    }

    SECTION("simple_test2")
    {
        uint8_t cmp_buf[1024], decomp_buf[1024];
        uLong cmp_len = sizeof(cmp_buf);

        const char *p = "This is a test.This is a test.This is a test.1234567This is a test.This is a test.123456";
        uLong uncomp_len = (uLong)strlen(p);

        int status = compress(cmp_buf, &cmp_len, (const uint8_t *)p, uncomp_len);
        REQUIRE(status == Z_OK);

        REQUIRE(cmp_len <= compressBound(uncomp_len));

        uLong decomp_len = sizeof(decomp_buf);
        status = uncompress(decomp_buf, &decomp_len, cmp_buf, cmp_len);
        ;

        REQUIRE(status == Z_OK);
        REQUIRE(decomp_len == uncomp_len);
        REQUIRE(memcmp(decomp_buf, p, uncomp_len) == 0);
    }
}

namespace
{
    struct PngChunk
    {
        std::string type;
        std::vector<uint8_t> data;
    };

    uint32_t read_be32(const uint8_t *p)
    {
        return ((uint32_t)p[0] << 24) | ((uint32_t)p[1] << 16) | ((uint32_t)p[2] << 8) | (uint32_t)p[3];
    }

    /* Splits a PNG file into chunks, verifying the signature and every chunk CRC. */
    std::vector<PngChunk> parse_png(const uint8_t *p, size_t len)
    {
        static const uint8_t sig[8] = { 0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a };
        std::vector<PngChunk> chunks;
        REQUIRE(len >= 8);
        REQUIRE(memcmp(p, sig, 8) == 0);
        size_t ofs = 8;
        while (ofs < len)
        {
            REQUIRE(len - ofs >= 12);
            uint32_t n = read_be32(p + ofs);
            REQUIRE(len - ofs - 12 >= n);
            REQUIRE(mz_crc32(MZ_CRC32_INIT, p + ofs + 4, n + 4) == read_be32(p + ofs + 8 + n));
            PngChunk c;
            c.type.assign((const char *)p + ofs + 4, 4);
            c.data.assign(p + ofs + 8, p + ofs + 8 + n);
            chunks.push_back(c);
            ofs += 12 + n;
        }
        REQUIRE(ofs == len);
        return chunks;
    }
}

TEST_CASE("PNG writer tests")
{
    const int w = 5, h = 3;

    SECTION("Indexed image with palette and transparency")
    {
        uint8_t idx[w * h];
        for (int i = 0; i < w * h; ++i)
            idx[i] = (uint8_t)(i % 3);
        const uint8_t plte[9] = { 255, 0, 0, 0, 255, 0, 0, 0, 255 };
        const uint8_t trns[2] = { 0, 128 };

        size_t png_len = 0;
        void *png = tdefl_write_image_to_png_file_in_memory_ex2(idx, w, h, 1, &png_len, 6, MZ_FALSE, plte, sizeof(plte), trns, sizeof(trns));
        REQUIRE(png != NULL);

        std::vector<PngChunk> chunks = parse_png((const uint8_t *)png, png_len);
        mz_free(png);

        REQUIRE(chunks.size() == 5);
        REQUIRE(chunks[0].type == "IHDR");
        REQUIRE(chunks[0].data.size() == 13);
        REQUIRE(read_be32(&chunks[0].data[0]) == (uint32_t)w);
        REQUIRE(read_be32(&chunks[0].data[4]) == (uint32_t)h);
        REQUIRE(chunks[0].data[8] == 8); /* bit depth */
        REQUIRE(chunks[0].data[9] == 3); /* color type: indexed */
        REQUIRE(chunks[1].type == "PLTE");
        REQUIRE(chunks[1].data == std::vector<uint8_t>(plte, plte + sizeof(plte)));
        REQUIRE(chunks[2].type == "tRNS");
        REQUIRE(chunks[2].data == std::vector<uint8_t>(trns, trns + sizeof(trns)));
        REQUIRE(chunks[3].type == "IDAT");
        REQUIRE(chunks[4].type == "IEND");
        REQUIRE(chunks[4].data.empty());

        uint8_t raw[(w + 1) * h];
        mz_ulong raw_len = sizeof(raw);
        REQUIRE(mz_uncompress(raw, &raw_len, chunks[3].data.data(), (mz_ulong)chunks[3].data.size()) == MZ_OK);
        REQUIRE(raw_len == sizeof(raw));
        for (int y = 0; y < h; ++y)
        {
            REQUIRE(raw[y * (w + 1)] == 0); /* filter type: none */
            REQUIRE(memcmp(raw + y * (w + 1) + 1, idx + y * w, w) == 0);
        }
    }

    SECTION("_ex2 without palette matches _ex")
    {
        uint8_t rgb[w * h * 3];
        for (int i = 0; i < (int)sizeof(rgb); ++i)
            rgb[i] = (uint8_t)(i * 7);

        size_t len1 = 0, len2 = 0;
        void *png1 = tdefl_write_image_to_png_file_in_memory_ex(rgb, w, h, 3, &len1, 6, MZ_TRUE);
        void *png2 = tdefl_write_image_to_png_file_in_memory_ex2(rgb, w, h, 3, &len2, 6, MZ_TRUE, NULL, 0, NULL, 0);
        REQUIRE(png1 != NULL);
        REQUIRE(png2 != NULL);
        REQUIRE(len1 == len2);
        REQUIRE(memcmp(png1, png2, len1) == 0);

        std::vector<PngChunk> chunks = parse_png((const uint8_t *)png1, len1);
        REQUIRE(chunks.size() == 3);
        REQUIRE(chunks[0].type == "IHDR");
        REQUIRE(chunks[0].data[9] == 2); /* color type: RGB */
        REQUIRE(chunks[1].type == "IDAT");
        REQUIRE(chunks[2].type == "IEND");
        mz_free(png1);
        mz_free(png2);
    }

    SECTION("Invalid palette parameters are rejected")
    {
        uint8_t img[w * h * 3] = { 0 };
        const uint8_t plte[9] = { 0 };
        const uint8_t trns[4] = { 0 };
        size_t len = 1;

        /* palette with more than one channel */
        REQUIRE(tdefl_write_image_to_png_file_in_memory_ex2(img, w, h, 3, &len, 6, MZ_FALSE, plte, 9, NULL, 0) == NULL);
        REQUIRE(len == 0);
        /* palette size not a multiple of 3 */
        REQUIRE(tdefl_write_image_to_png_file_in_memory_ex2(img, w, h, 1, &len, 6, MZ_FALSE, plte, 8, NULL, 0) == NULL);
        /* palette larger than 256 entries */
        REQUIRE(tdefl_write_image_to_png_file_in_memory_ex2(img, w, h, 1, &len, 6, MZ_FALSE, plte, 257 * 3, NULL, 0) == NULL);
        /* more transparency entries than palette entries */
        REQUIRE(tdefl_write_image_to_png_file_in_memory_ex2(img, w, h, 1, &len, 6, MZ_FALSE, plte, 9, trns, 4) == NULL);
        /* transparency without palette */
        REQUIRE(tdefl_write_image_to_png_file_in_memory_ex2(img, w, h, 1, &len, 6, MZ_FALSE, NULL, 0, trns, 1) == NULL);
        /* NULL pointers with non-zero sizes */
        REQUIRE(tdefl_write_image_to_png_file_in_memory_ex2(img, w, h, 1, &len, 6, MZ_FALSE, NULL, 9, NULL, 0) == NULL);
        REQUIRE(tdefl_write_image_to_png_file_in_memory_ex2(img, w, h, 1, &len, 6, MZ_FALSE, plte, 9, NULL, 1) == NULL);
        /* negative sizes */
        REQUIRE(tdefl_write_image_to_png_file_in_memory_ex2(img, w, h, 1, &len, 6, MZ_FALSE, plte, -3, NULL, 0) == NULL);
    }
}
