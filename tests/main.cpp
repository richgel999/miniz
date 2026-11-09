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

#ifndef MINIZ_NO_TIME
static MZ_TIME_T local_time(int year, int mon, int mday, int hour, int min, int sec)
{
    struct tm tm = {};
    tm.tm_year = year - 1900;
    tm.tm_mon = mon - 1;
    tm.tm_mday = mday;
    tm.tm_hour = hour;
    tm.tm_min = min;
    tm.tm_sec = sec;
    tm.tm_isdst = -1;
    return mktime(&tm);
}

TEST_CASE("Zip writer clamps times outside the DOS date range")
{
    MZ_TIME_T before = local_time(1975, 5, 22, 14, 13, 20);
    MZ_TIME_T after = local_time(2110, 1, 1, 6, 13, 20);

    mz_zip_archive writer = {};
    REQUIRE(mz_zip_writer_init_heap(&writer, 0, 0));
    REQUIRE(mz_zip_writer_add_mem_ex_v2(&writer, "before.txt", "a", 1, NULL, 0, MZ_DEFAULT_COMPRESSION, 0, 0, &before, NULL, 0, NULL, 0));
    REQUIRE(mz_zip_writer_add_mem_ex_v2(&writer, "after.txt", "b", 1, NULL, 0, MZ_DEFAULT_COMPRESSION, 0, 0, &after, NULL, 0, NULL, 0));
    void *buf = NULL;
    size_t size = 0;
    REQUIRE(mz_zip_writer_finalize_heap_archive(&writer, &buf, &size));
    REQUIRE(mz_zip_writer_end(&writer));

    mz_zip_archive reader = {};
    REQUIRE(mz_zip_reader_init_mem(&reader, buf, size, 0));
    mz_zip_archive_file_stat stat;
    REQUIRE(mz_zip_reader_file_stat(&reader, 0, &stat));
    REQUIRE(stat.m_time == local_time(1980, 1, 1, 0, 0, 0));
    REQUIRE(mz_zip_reader_file_stat(&reader, 1, &stat));
    REQUIRE(stat.m_time == local_time(2107, 12, 31, 23, 59, 58));
    REQUIRE(mz_zip_reader_end(&reader));
    mz_free(buf);
}
#endif /* MINIZ_NO_TIME */

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

TEST_CASE("Zip reader zip64 local header offset out of bounds")
{
    /* A central directory entry whose 32-bit comp/uncomp/local-header-offset
     * fields are all 0xFFFFFFFF forces the reader to pull the real values from
     * the zip64 extended-information extra field.  Here that field reports a
     * local header offset far beyond the end of the archive.  The offset is not
     * range-checked before the local header is read, so it reaches the memory
     * read callback.  mz_zip_mem_read_func() clamps the copy length to 0 but
     * still formed the source pointer "pMem + file_ofs"; with a huge offset
     * that pointer arithmetic overflows (UB; a SIGILL/trap under
     * -fsanitize=pointer-overflow).  The extract must fail cleanly instead. */
    static const unsigned char archive[] = {
        0x50, 0x4b, 0x01, 0x02, 0x14, 0x00, 0x14, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0x01, 0x00, 0x1c, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0x41, 0x01,
        0x00, 0x18, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf0, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0x50, 0x4b, 0x05, 0x06, 0x00, 0x00, 0x00, 0x00, 0x01,
        0x00, 0x01, 0x00, 0x4b, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00,
    };

    mz_zip_archive zip = {};
    REQUIRE(mz_zip_reader_init_mem(&zip, archive, sizeof(archive), 0));
    REQUIRE(mz_zip_reader_get_num_files(&zip) == 1);

    size_t out_size = 0;
    void *p = mz_zip_reader_extract_to_heap(&zip, 0, &out_size, 0);
    REQUIRE(p == nullptr);
    REQUIRE(zip.m_last_error == MZ_ZIP_FILE_READ_FAILED);

    free(p);
    mz_zip_reader_end(&zip);
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
        REQUIRE(tdefl_write_image_to_png_file_in_memory_ex2(img, w, h, 1, &len, 6, MZ_FALSE, plte, 9, trns, -1) == NULL);
    }

    SECTION("Invalid image dimensions and channel counts are rejected")
    {
        uint8_t img[w * h * 4] = { 0 };
        size_t len;

        const int bad_dims[][2] = { { 0, 1 }, { 1, 0 }, { -1, 1 }, { 1, -1 }, { 0x10000, 1 }, { 1, 0x10000 } };
        for (size_t i = 0; i < sizeof(bad_dims) / sizeof(bad_dims[0]); ++i)
        {
            INFO("w=" << bad_dims[i][0] << " h=" << bad_dims[i][1]);
            len = 1;
            REQUIRE(tdefl_write_image_to_png_file_in_memory_ex2(img, bad_dims[i][0], bad_dims[i][1], 1, &len, 6, MZ_FALSE, NULL, 0, NULL, 0) == NULL);
            REQUIRE(len == 0);
            len = 1;
            REQUIRE(tdefl_write_image_to_png_file_in_memory_ex(img, bad_dims[i][0], bad_dims[i][1], 1, &len, 6, MZ_FALSE) == NULL);
            REQUIRE(len == 0);
        }

        const int bad_chans[] = { -1, 0, 5 };
        for (size_t i = 0; i < sizeof(bad_chans) / sizeof(bad_chans[0]); ++i)
        {
            INFO("num_chans=" << bad_chans[i]);
            len = 1;
            REQUIRE(tdefl_write_image_to_png_file_in_memory_ex2(img, w, h, bad_chans[i], &len, 6, MZ_FALSE, NULL, 0, NULL, 0) == NULL);
            REQUIRE(len == 0);
            len = 1;
            REQUIRE(tdefl_write_image_to_png_file_in_memory(img, w, h, bad_chans[i], &len) == NULL);
            REQUIRE(len == 0);
        }
    }

    SECTION("Maximum image dimensions are accepted")
    {
        const int max_dim = 0xFFFF;
        std::vector<uint8_t> line(max_dim);
        for (int i = 0; i < max_dim; ++i)
            line[i] = (uint8_t)(i * 31);

        /* max_dim x 1 and 1 x max_dim grayscale images contain the same bytes */
        const int dims[][2] = { { max_dim, 1 }, { 1, max_dim } };
        for (int d = 0; d < 2; ++d)
        {
            const int iw = dims[d][0], ih = dims[d][1];
            INFO("w=" << iw << " h=" << ih);
            size_t len = 0;
            void *png = tdefl_write_image_to_png_file_in_memory_ex2(line.data(), iw, ih, 1, &len, 1, MZ_FALSE, NULL, 0, NULL, 0);
            REQUIRE(png != NULL);
            std::vector<PngChunk> chunks = parse_png((const uint8_t *)png, len);
            mz_free(png);

            REQUIRE(chunks.size() == 3);
            REQUIRE(chunks[0].type == "IHDR");
            REQUIRE(read_be32(&chunks[0].data[0]) == (uint32_t)iw);
            REQUIRE(read_be32(&chunks[0].data[4]) == (uint32_t)ih);
            REQUIRE(chunks[1].type == "IDAT");

            std::vector<uint8_t> raw((size_t)(iw + 1) * ih);
            mz_ulong raw_len = (mz_ulong)raw.size();
            REQUIRE(mz_uncompress(raw.data(), &raw_len, chunks[1].data.data(), (mz_ulong)chunks[1].data.size()) == MZ_OK);
            REQUIRE(raw_len == raw.size());
            for (int y = 0; y < ih; ++y)
            {
                REQUIRE(raw[(size_t)y * (iw + 1)] == 0); /* filter type: none */
                REQUIRE(memcmp(&raw[(size_t)y * (iw + 1) + 1], &line[(size_t)y * iw], iw) == 0);
            }
        }
    }
}
