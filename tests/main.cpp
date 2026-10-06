#include "catch_amalgamated.hpp"
#include "../miniz.h"
#include "miniz_zip.h"
#include <assert.h>
#include <string>
#include <vector>
#include <memory>
#include <unordered_set>

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

struct iterator_test_allocator
{
    std::unordered_set<void *> allocations;
    bool fail_dictionary = false;
    bool dictionary_failed = false;
    unsigned invalid_frees = 0;

    static void *alloc(void *opaque, size_t items, size_t size)
    {
        auto &self = *static_cast<iterator_test_allocator *>(opaque);
        if (self.fail_dictionary && items * size == TINFL_LZ_DICT_SIZE)
        {
            self.dictionary_failed = true;
            return nullptr;
        }
        void *p = malloc(items * size);
        if (p)
            self.allocations.insert(p);
        return p;
    }

    static void *realloc(void *opaque, void *p, size_t items, size_t size)
    {
        auto &self = *static_cast<iterator_test_allocator *>(opaque);
        self.allocations.erase(p);
        void *result = ::realloc(p, items * size);
        if (result)
            self.allocations.insert(result);
        else if (p)
            self.allocations.insert(p);
        return result;
    }

    static void free(void *opaque, void *p)
    {
        auto &self = *static_cast<iterator_test_allocator *>(opaque);
        if (!p)
            return;
        if (self.allocations.erase(p))
            ::free(p);
        else
            ++self.invalid_frees;
    }
};

TEST_CASE("Zip extraction iterator buffer ownership")
{
    const bool memory_reader = GENERATE(false, true);
    const bool fail_dictionary = GENERATE(false, true);
    CAPTURE(memory_reader, fail_dictionary);

    const std::string contents(4096, 'a');
    mz_zip_archive writer = {};
    REQUIRE(mz_zip_writer_init_heap(&writer, 0, 0));
    REQUIRE(mz_zip_writer_add_mem(&writer, "test.txt", contents.data(), contents.size(), MZ_BEST_COMPRESSION));
    void *archive = nullptr;
    size_t archive_size = 0;
    REQUIRE(mz_zip_writer_finalize_heap_archive(&writer, &archive, &archive_size));
    REQUIRE(mz_zip_writer_end(&writer));
    std::unique_ptr<void, decltype(&mz_free)> archive_owner(archive, mz_free);
    std::string_view archive_view(static_cast<const char *>(archive), archive_size);

    iterator_test_allocator allocator;
    mz_zip_archive reader = {};
    reader.m_pAlloc = iterator_test_allocator::alloc;
    reader.m_pRealloc = iterator_test_allocator::realloc;
    reader.m_pFree = iterator_test_allocator::free;
    reader.m_pAlloc_opaque = &allocator;
    if (memory_reader)
        REQUIRE(mz_zip_reader_init_mem(&reader, archive, archive_size, 0));
    else
    {
        reader.m_pIO_opaque = &archive_view;
        reader.m_pRead = [](void *opaque, mz_uint64 offset, void *buf, size_t size) -> size_t {
            const auto &view = *static_cast<std::string_view *>(opaque);
            if (offset > view.size() || size > view.size() - offset)
                return 0;
            memcpy(buf, view.data() + offset, size);
            return size;
        };
        REQUIRE(mz_zip_reader_init(&reader, archive_size, 0));
    }
    mz_zip_archive_file_stat stat;
    REQUIRE(mz_zip_reader_file_stat(&reader, 0, &stat));
    REQUIRE(stat.m_method == MZ_DEFLATED);

    const auto reader_allocations = allocator.allocations.size();
    allocator.fail_dictionary = fail_dictionary;
    auto *iter = mz_zip_reader_extract_iter_new(&reader, 0, 0);
    if (fail_dictionary)
    {
        CHECK(iter == nullptr);
        CHECK(allocator.dictionary_failed);
        CHECK(mz_zip_get_last_error(&reader) == MZ_ZIP_ALLOC_FAILED);
    }
    else
    {
        REQUIRE(iter != nullptr);
        std::string extracted(contents.size(), '\0');
        CHECK(mz_zip_reader_extract_iter_read(iter, extracted.data(), extracted.size()) == contents.size());
        CHECK(extracted == contents);
    }
    if (iter)
        CHECK(mz_zip_reader_extract_iter_free(iter));
    CHECK(allocator.allocations.size() == reader_allocations);
    CHECK(mz_zip_reader_end(&reader));
    CHECK(allocator.allocations.empty());
    CHECK(allocator.invalid_frees == 0);
}

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
