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

    SECTION("Test archive with end of central dir record at start of file")
    {
        /* Empty archive: a 22-byte end of central directory record at offset 0
         * followed by a 4075-byte comment, so the record is more than one scan
         * buffer away from the end of the 4097-byte file. */
        const unsigned char eocd[22] = { 0x50, 0x4b, 0x05, 0x06, 0, 0, 0, 0, 0, 0, 0,
                                         0, 0, 0, 0, 0, 0, 0, 0, 0, 0xeb, 0x0f };
        std::vector<unsigned char> data(sizeof(eocd) + 4075, 'C');
        for (size_t i = 0; i < sizeof(eocd); ++i)
            data[i] = eocd[i];
        mz_zip_archive zip_archive = {};

        auto b = mz_zip_reader_init_mem(&zip_archive, data.data(), data.size(), 0);
        REQUIRE(b);
        REQUIRE(mz_zip_reader_get_num_files(&zip_archive) == 0);
        mz_zip_reader_end(&zip_archive);
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

#ifndef MINIZ_NO_ARCHIVE_WRITING_APIS
TEST_CASE("ZIP archive comments")
{
    const bool zip64 = GENERATE(false, true);
    const bool empty = GENERATE(false, true);
    const size_t comment_size = GENERATE(0, 3, 65535);
    std::vector<mz_uint8> comment(comment_size, 'x');
    if (comment_size)
        comment[1] = 0;
    mz_zip_archive zip = {};
    REQUIRE(mz_zip_writer_init_heap_v2(&zip, 0, 0, zip64 ? MZ_ZIP_FLAG_WRITE_ZIP64 : 0));
    if (!empty)
        REQUIRE(mz_zip_writer_add_mem(&zip, "test.txt", "foo", 3, MZ_DEFAULT_COMPRESSION));
    void *buffer = nullptr;
    size_t size = 0;
    REQUIRE(mz_zip_writer_finalize_heap_archive_v2(&zip, &buffer, &size, comment.data(), static_cast<mz_uint16>(comment.size())));
    REQUIRE(zip.m_archive_size == size);
    REQUIRE(mz_zip_writer_end(&zip));

    const auto *bytes = static_cast<const mz_uint8 *>(buffer);
    const size_t eocd = size - comment.size() - 22;
    REQUIRE(memcmp(bytes + eocd, "PK\5\6", 4) == 0);
    REQUIRE((bytes[eocd + 20] | (bytes[eocd + 21] << 8)) == comment.size());
    if (comment_size)
        REQUIRE(memcmp(bytes + eocd + 22, comment.data(), comment.size()) == 0);
    REQUIRE(mz_zip_reader_init_mem(&zip, buffer, size, 0));
    REQUIRE(mz_zip_reader_get_num_files(&zip) == (empty ? 0 : 1));
    if (!empty)
    {
        char contents[3];
        REQUIRE(mz_zip_reader_extract_file_to_mem(&zip, "test.txt", contents, sizeof(contents), 0));
        REQUIRE(memcmp(contents, "foo", 3) == 0);
    }
    REQUIRE(mz_zip_reader_end(&zip));
    mz_free(buffer);
}

static size_t reject_archive_comment(void *, mz_uint64, const void *, size_t size)
{
    return size == 3 ? 2 : size;
}

TEST_CASE("ZIP archive comment errors")
{
    mz_zip_archive zip = {};
    zip.m_pWrite = reject_archive_comment;
    REQUIRE(mz_zip_writer_init(&zip, 0));
    REQUIRE_FALSE(mz_zip_writer_finalize_archive_v2(&zip, nullptr, 3));
    REQUIRE(mz_zip_get_last_error(&zip) == MZ_ZIP_INVALID_PARAMETER);
    REQUIRE(zip.m_archive_size == 0);
    REQUIRE_FALSE(mz_zip_writer_finalize_archive_v2(&zip, "abc", 3));
    REQUIRE(mz_zip_get_last_error(&zip) == MZ_ZIP_FILE_WRITE_FAILED);
    REQUIRE(zip.m_zip_mode == MZ_ZIP_MODE_WRITING);
    REQUIRE(mz_zip_writer_end(&zip));
}

#ifndef MINIZ_NO_STDIO
TEST_CASE("ZIP archive comment replacement when appending")
{
    const char *filename = "comment-test.zip";
    const bool zip64 = GENERATE(false, true);
    const std::string new_comment = GENERATE(std::string(), std::string("new comment"), std::string(65535, 'n'));
    mz_zip_archive zip = {};
    REQUIRE(mz_zip_writer_init_file_v2(&zip, filename, 0, zip64 ? MZ_ZIP_FLAG_WRITE_ZIP64 : 0));
    REQUIRE(mz_zip_writer_add_mem(&zip, "first", "a", 1, 0));
    const std::string old_comment(4096, 'x');
    REQUIRE(mz_zip_writer_finalize_archive_v2(&zip, old_comment.data(), static_cast<mz_uint16>(old_comment.size())));
    REQUIRE(mz_zip_writer_end(&zip));
    REQUIRE(mz_zip_reader_init_file(&zip, filename, MZ_ZIP_FLAG_READ_ALLOW_WRITING));
    REQUIRE(mz_zip_writer_init_from_reader(&zip, filename));
    REQUIRE(mz_zip_writer_add_mem(&zip, "second", "b", 1, 0));
    REQUIRE(mz_zip_writer_finalize_archive_v2(&zip, new_comment.data(), static_cast<mz_uint16>(new_comment.size())));
    REQUIRE(mz_zip_writer_end(&zip));
    REQUIRE(mz_zip_reader_init_file(&zip, filename, 0));
    REQUIRE(mz_zip_reader_get_num_files(&zip) == 2);
    std::string comment(new_comment.size(), '\0');
    if (!comment.empty())
    {
        REQUIRE(mz_zip_read_archive_data(&zip, zip.m_archive_size - comment.size(), comment.data(), comment.size()) == comment.size());
        REQUIRE(comment == new_comment);
    }
    mz_uint8 eocd[22];
    REQUIRE(mz_zip_read_archive_data(&zip, zip.m_archive_size - comment.size() - sizeof(eocd), eocd, sizeof(eocd)) == sizeof(eocd));
    REQUIRE(memcmp(eocd, "PK\5\6", 4) == 0);
    REQUIRE((eocd[20] | (eocd[21] << 8)) == comment.size());
    REQUIRE(mz_zip_reader_end(&zip));
    REQUIRE(unlink(filename) == 0);
}
#endif
#endif

#ifndef MINIZ_NO_ARCHIVE_WRITING_APIS
TEST_CASE("Heap ZIP comments can shrink without leaving an old end record")
{
    const bool zip64 = GENERATE(false, true);
    const bool empty = GENERATE(false, true);
    mz_zip_archive zip = {};
    const std::string old_comment(4096, 'x');
    REQUIRE(mz_zip_writer_init_heap_v2(&zip, 0, 0, zip64 ? MZ_ZIP_FLAG_WRITE_ZIP64 : 0));
    if (!empty)
        REQUIRE(mz_zip_writer_add_mem(&zip, "first", "a", 1, 0));
    void *buffer = nullptr;
    size_t size = 0;
    REQUIRE(mz_zip_writer_finalize_heap_archive_v2(&zip, &buffer, &size, old_comment.data(), static_cast<mz_uint16>(old_comment.size())));
    REQUIRE(mz_zip_writer_end(&zip));
    REQUIRE(mz_zip_reader_init_mem(&zip, buffer, size, 0));
    REQUIRE(mz_zip_writer_init_from_reader(&zip, nullptr));
    REQUIRE(mz_zip_writer_finalize_heap_archive(&zip, &buffer, &size));
    REQUIRE(zip.m_archive_size == size);
    REQUIRE(mz_zip_writer_end(&zip));
    const auto *bytes = static_cast<const mz_uint8 *>(buffer);
    REQUIRE(memcmp(bytes + size - 22, "PK\5\6", 4) == 0);
    REQUIRE(bytes[size - 2] == 0);
    REQUIRE(bytes[size - 1] == 0);
    REQUIRE(mz_zip_reader_init_mem(&zip, buffer, size, 0));
    REQUIRE(mz_zip_reader_get_num_files(&zip) == (empty ? 0 : 1));
    REQUIRE(mz_zip_reader_end(&zip));
    mz_free(buffer);
}
#endif
