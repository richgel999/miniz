/* Deterministic PNG writer test cases shared by the libpng and Python round-trip tests. */
#ifndef MINIZ_PNG_TEST_CASES_H
#define MINIZ_PNG_TEST_CASES_H

#include <stddef.h>
#include <string.h>

enum
{
    PNG_TC_GRADIENT = 0, /* smooth, compresses well */
    PNG_TC_NOISE = 1,    /* pseudo random, mostly incompressible */
    PNG_TC_CONSTANT = 2  /* single value, extremely compressible */
};

typedef struct
{
    const char *name;
    int w, h, chans;
    int level;
    int flip;
    int plte_entries; /* > 0: indexed image (chans must be 1) */
    int trns_entries; /* <= plte_entries */
    int pattern;
} png_test_case;

static const png_test_case g_png_test_cases[] = {
    /* tiny images, every channel count */
    { "gray_1x1", 1, 1, 1, 6, 0, 0, 0, PNG_TC_NOISE },
    { "ga_1x1", 1, 1, 2, 6, 0, 0, 0, PNG_TC_NOISE },
    { "rgb_1x1", 1, 1, 3, 6, 0, 0, 0, PNG_TC_NOISE },
    { "rgba_1x1", 1, 1, 4, 6, 0, 0, 0, PNG_TC_NOISE },
    /* small odd sizes, with and without flip */
    { "gray_7x5", 7, 5, 1, 6, 0, 0, 0, PNG_TC_GRADIENT },
    { "gray_7x5_flip", 7, 5, 1, 6, 1, 0, 0, PNG_TC_GRADIENT },
    { "ga_7x5", 7, 5, 2, 6, 0, 0, 0, PNG_TC_GRADIENT },
    { "ga_7x5_flip", 7, 5, 2, 6, 1, 0, 0, PNG_TC_NOISE },
    { "rgb_7x5", 7, 5, 3, 6, 0, 0, 0, PNG_TC_GRADIENT },
    { "rgb_7x5_flip", 7, 5, 3, 6, 1, 0, 0, PNG_TC_NOISE },
    { "rgba_7x5", 7, 5, 4, 6, 0, 0, 0, PNG_TC_GRADIENT },
    { "rgba_7x5_flip", 7, 5, 4, 6, 1, 0, 0, PNG_TC_NOISE },
    /* every compression level */
    { "rgb_64x64_l0", 64, 64, 3, 0, 0, 0, 0, PNG_TC_GRADIENT },
    { "rgb_64x64_l1", 64, 64, 3, 1, 0, 0, 0, PNG_TC_GRADIENT },
    { "rgb_64x64_l2", 64, 64, 3, 2, 0, 0, 0, PNG_TC_GRADIENT },
    { "rgb_64x64_l3", 64, 64, 3, 3, 0, 0, 0, PNG_TC_GRADIENT },
    { "rgb_64x64_l4", 64, 64, 3, 4, 0, 0, 0, PNG_TC_GRADIENT },
    { "rgb_64x64_l5", 64, 64, 3, 5, 0, 0, 0, PNG_TC_GRADIENT },
    { "rgb_64x64_l6", 64, 64, 3, 6, 0, 0, 0, PNG_TC_GRADIENT },
    { "rgb_64x64_l7", 64, 64, 3, 7, 0, 0, 0, PNG_TC_GRADIENT },
    { "rgb_64x64_l8", 64, 64, 3, 8, 0, 0, 0, PNG_TC_GRADIENT },
    { "rgb_64x64_l9", 64, 64, 3, 9, 0, 0, 0, PNG_TC_GRADIENT },
    { "rgb_64x64_l10", 64, 64, 3, 10, 0, 0, 0, PNG_TC_GRADIENT },
    { "rgb_64x64_l99", 64, 64, 3, 99, 0, 0, 0, PNG_TC_GRADIENT }, /* clamped to 10 */
    /* degenerate shapes */
    { "gray_1x300", 1, 300, 1, 6, 0, 0, 0, PNG_TC_GRADIENT },
    { "ga_1024x1", 1024, 1, 2, 6, 0, 0, 0, PNG_TC_NOISE },
    { "rgba_257x3", 257, 3, 4, 6, 1, 0, 0, PNG_TC_NOISE },
    { "rgb_100x100_const", 100, 100, 3, 9, 0, 0, 0, PNG_TC_CONSTANT },
    /* larger images: many deflate blocks, stored blocks > 64 KiB */
    { "rgba_300x300_l0_noise", 300, 300, 4, 0, 0, 0, 0, PNG_TC_NOISE },
    { "rgb_512x512_l1_noise", 512, 512, 3, 1, 0, 0, 0, PNG_TC_NOISE },
    { "rgba_640x480_l6", 640, 480, 4, 6, 1, 0, 0, PNG_TC_GRADIENT },
    /* indexed images */
    { "pal1_9x9", 9, 9, 1, 6, 0, 1, 0, PNG_TC_NOISE },
    { "pal2_9x9_trns1", 9, 9, 1, 6, 0, 2, 1, PNG_TC_NOISE },
    { "pal16_33x17_trns16", 33, 17, 1, 6, 1, 16, 16, PNG_TC_GRADIENT },
    { "pal256_64x64", 64, 64, 1, 6, 0, 256, 0, PNG_TC_NOISE },
    { "pal256_64x64_trns100", 64, 64, 1, 6, 1, 256, 100, PNG_TC_NOISE },
    { "pal256_200x150_l0_trns256", 200, 150, 1, 0, 0, 256, 256, PNG_TC_GRADIENT },
};

static const size_t g_png_num_test_cases = sizeof(g_png_test_cases) / sizeof(g_png_test_cases[0]);

static unsigned png_tc_rand(unsigned *state)
{
    *state = *state * 1103515245u + 12345u;
    return *state >> 16;
}

/* Fills img (w*h*chans bytes) with the source image passed to the PNG writer. */
static void png_tc_fill_image(const png_test_case *tc, unsigned char *img)
{
    unsigned seed = 0x1234u + (unsigned)tc->w * 31u + (unsigned)tc->h * 17u + (unsigned)tc->chans;
    int x, y, c;
    for (y = 0; y < tc->h; ++y)
        for (x = 0; x < tc->w; ++x)
            for (c = 0; c < tc->chans; ++c)
            {
                unsigned v;
                if (tc->pattern == PNG_TC_NOISE)
                    v = png_tc_rand(&seed);
                else if (tc->pattern == PNG_TC_CONSTANT)
                    v = 0x5a + (unsigned)c;
                else
                    v = (unsigned)(x * 3 + y * 5 + c * 64);
                if (tc->plte_entries > 0)
                    v %= (unsigned)tc->plte_entries;
                img[((size_t)y * tc->w + x) * tc->chans + c] = (unsigned char)v;
            }
}

/* Fills plte (plte_entries*3 bytes) and trns (trns_entries bytes). */
static void png_tc_fill_palette(const png_test_case *tc, unsigned char *plte, unsigned char *trns)
{
    int i;
    for (i = 0; i < tc->plte_entries; ++i)
    {
        plte[i * 3 + 0] = (unsigned char)(i * 7);
        plte[i * 3 + 1] = (unsigned char)(255 - i);
        plte[i * 3 + 2] = (unsigned char)(i * 13 + 5);
    }
    for (i = 0; i < tc->trns_entries; ++i)
        trns[i] = (unsigned char)(i * 37 + 3);
}

#endif
