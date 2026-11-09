/* Round-trip tests: PNGs written by miniz are decoded with libpng and compared to the source image. */
#include "catch_amalgamated.hpp"
#include "miniz.h"
#include "png_test_cases.h"

#include <png.h>

#include <setjmp.h>
#include <stdint.h>
#include <string.h>
#include <string>
#include <vector>

namespace
{
    struct MemReader
    {
        const uint8_t *data;
        size_t size;
        size_t pos;
    };

    void mem_read_fn(png_structp png, png_bytep out, png_size_t len)
    {
        MemReader *r = (MemReader *)png_get_io_ptr(png);
        if (r->size - r->pos < len)
            png_error(png, "read past end of PNG data");
        memcpy(out, r->data + r->pos, len);
        r->pos += len;
    }

    void error_fn(png_structp png, png_const_charp msg)
    {
        std::string *err = (std::string *)png_get_error_ptr(png);
        *err = msg;
        png_longjmp(png, 1);
    }

    void warning_fn(png_structp png, png_const_charp msg)
    {
        /* Treat warnings as failures: a conforming PNG should not produce any. */
        std::string *err = (std::string *)png_get_error_ptr(png);
        if (err->empty())
            *err = std::string("warning: ") + msg;
    }

    struct Decoded
    {
        std::string error;
        png_uint_32 w = 0, h = 0;
        int bit_depth = 0, color_type = 0, interlace = 0, compression = 0, filter = 0;
        std::vector<uint8_t> pixels; /* h rows of rowbytes each */
        size_t rowbytes = 0;
        std::vector<uint8_t> plte; /* RGB triplets */
        std::vector<uint8_t> trns;
        size_t bytes_consumed = 0;
    };

    /* Decodes without any transformations so the stored format can be checked exactly. */
    void decode_with_libpng(const uint8_t *data, size_t size, Decoded &out)
    {
        MemReader reader = { data, size, 0 };
        png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, &out.error, error_fn, warning_fn);
        if (!png)
        {
            out.error = "png_create_read_struct failed";
            return;
        }
        png_infop info = png_create_info_struct(png);
        png_infop end_info = png_create_info_struct(png);
        std::vector<png_bytep> rows;
        if (!info || !end_info)
        {
            out.error = "png_create_info_struct failed";
            png_destroy_read_struct(&png, info ? &info : NULL, end_info ? &end_info : NULL);
            return;
        }
        if (setjmp(png_jmpbuf(png)))
        {
            png_destroy_read_struct(&png, &info, &end_info);
            return;
        }
        /* Be strict: benign errors become errors, bad CRCs are fatal. */
        png_set_benign_errors(png, 0);
        png_set_crc_action(png, PNG_CRC_ERROR_QUIT, PNG_CRC_ERROR_QUIT);
        png_set_read_fn(png, &reader, mem_read_fn);
        png_read_info(png, info);
        png_get_IHDR(png, info, &out.w, &out.h, &out.bit_depth, &out.color_type, &out.interlace, &out.compression, &out.filter);

        png_colorp palette = NULL;
        int num_palette = 0;
        if (png_get_PLTE(png, info, &palette, &num_palette) & PNG_INFO_PLTE)
        {
            for (int i = 0; i < num_palette; ++i)
            {
                out.plte.push_back(palette[i].red);
                out.plte.push_back(palette[i].green);
                out.plte.push_back(palette[i].blue);
            }
        }
        png_bytep trans_alpha = NULL;
        int num_trans = 0;
        png_color_16p trans_color = NULL;
        if (png_get_tRNS(png, info, &trans_alpha, &num_trans, &trans_color) & PNG_INFO_tRNS)
        {
            if (trans_alpha)
                out.trns.assign(trans_alpha, trans_alpha + num_trans);
        }

        out.rowbytes = png_get_rowbytes(png, info);
        out.pixels.resize(out.rowbytes * out.h);
        rows.resize(out.h);
        for (png_uint_32 y = 0; y < out.h; ++y)
            rows[y] = out.pixels.data() + y * out.rowbytes;
        png_read_image(png, rows.data());
        png_read_end(png, end_info);
        out.bytes_consumed = reader.pos;
        png_destroy_read_struct(&png, &info, &end_info);
    }

    std::vector<uint8_t> write_png(const png_test_case &tc, const std::vector<uint8_t> &img,
                                   const std::vector<uint8_t> &plte, const std::vector<uint8_t> &trns)
    {
        size_t len = 0;
        void *p = tdefl_write_image_to_png_file_in_memory_ex2(img.data(), tc.w, tc.h, tc.chans, &len, (mz_uint)tc.level, tc.flip ? MZ_TRUE : MZ_FALSE,
                                                              plte.empty() ? NULL : plte.data(), (int)plte.size(),
                                                              trns.empty() ? NULL : trns.data(), (int)trns.size());
        REQUIRE(p != NULL);
        REQUIRE(len > 0);
        std::vector<uint8_t> out((const uint8_t *)p, (const uint8_t *)p + len);
        mz_free(p);
        return out;
    }
}

TEST_CASE("PNG round-trip through libpng")
{
    for (size_t i = 0; i < g_png_num_test_cases; ++i)
    {
        const png_test_case &tc = g_png_test_cases[i];
        DYNAMIC_SECTION(tc.name)
        {
            std::vector<uint8_t> img((size_t)tc.w * tc.h * tc.chans);
            png_tc_fill_image(&tc, img.data());
            std::vector<uint8_t> plte((size_t)tc.plte_entries * 3), trns((size_t)tc.trns_entries);
            png_tc_fill_palette(&tc, plte.data(), trns.data());

            std::vector<uint8_t> png = write_png(tc, img, plte, trns);

            Decoded d;
            decode_with_libpng(png.data(), png.size(), d);
            INFO("libpng: " << d.error);
            REQUIRE(d.error.empty());
            REQUIRE(d.bytes_consumed == png.size());

            static const int color_types[] = { 0, PNG_COLOR_TYPE_GRAY, PNG_COLOR_TYPE_GRAY_ALPHA, PNG_COLOR_TYPE_RGB, PNG_COLOR_TYPE_RGB_ALPHA };
            REQUIRE(d.w == (png_uint_32)tc.w);
            REQUIRE(d.h == (png_uint_32)tc.h);
            REQUIRE(d.bit_depth == 8);
            REQUIRE(d.color_type == (tc.plte_entries > 0 ? PNG_COLOR_TYPE_PALETTE : color_types[tc.chans]));
            REQUIRE(d.interlace == PNG_INTERLACE_NONE);
            REQUIRE(d.compression == PNG_COMPRESSION_TYPE_BASE);
            REQUIRE(d.filter == PNG_FILTER_TYPE_BASE);
            REQUIRE(d.plte == plte);
            REQUIRE(d.trns == trns);

            const size_t bpl = (size_t)tc.w * tc.chans;
            REQUIRE(d.rowbytes == bpl);
            for (int y = 0; y < tc.h; ++y)
            {
                const int src_y = tc.flip ? tc.h - 1 - y : y;
                INFO("row " << y);
                REQUIRE(memcmp(d.pixels.data() + y * bpl, img.data() + src_y * bpl, bpl) == 0);
            }
        }
    }
}

TEST_CASE("libpng rejects corrupted miniz PNGs")
{
    /* Sanity check that the libpng harness actually detects damage. */
    const png_test_case &tc = g_png_test_cases[4]; /* gray_7x5 */
    std::vector<uint8_t> img((size_t)tc.w * tc.h * tc.chans);
    png_tc_fill_image(&tc, img.data());
    std::vector<uint8_t> png = write_png(tc, img, std::vector<uint8_t>(), std::vector<uint8_t>());

    SECTION("bad IHDR CRC")
    {
        png[29] ^= 1;
        Decoded d;
        decode_with_libpng(png.data(), png.size(), d);
        REQUIRE(!d.error.empty());
    }
    SECTION("corrupted IDAT payload")
    {
        /* Flip a byte inside the deflate stream and fix up the chunk CRC so only the zlib layer can notice. */
        const size_t idat = 33;
        uint32_t n = ((uint32_t)png[idat] << 24) | ((uint32_t)png[idat + 1] << 16) | ((uint32_t)png[idat + 2] << 8) | png[idat + 3];
        REQUIRE(memcmp(&png[idat + 4], "IDAT", 4) == 0);
        png[idat + 8 + n - 1] ^= 0xff; /* last byte of the adler32 */
        mz_ulong crc = mz_crc32(MZ_CRC32_INIT, &png[idat + 4], n + 4);
        for (int i = 0; i < 4; ++i)
            png[idat + 8 + n + i] = (uint8_t)(crc >> (24 - 8 * i));
        Decoded d;
        decode_with_libpng(png.data(), png.size(), d);
        REQUIRE(!d.error.empty());
    }
    SECTION("truncated file")
    {
        png.resize(png.size() - 20);
        Decoded d;
        decode_with_libpng(png.data(), png.size(), d);
        REQUIRE(!d.error.empty());
    }
}
