/* Writes the PNG round-trip test cases to a directory for verification by an external decoder
   (see png_roundtrip_check.py). For each case <name> it writes:
     <name>.png   the PNG produced by miniz
     <name>.raw   the source image passed to the writer (before flipping)
     <name>.plte  palette (only for indexed images)
     <name>.trns  transparency (only if present)
   and appends "<name> <w> <h> <chans> <flip> <plte_entries> <trns_entries>" to manifest.txt. */
#include "miniz.h"
#include "png_test_cases.h"

#include <stdio.h>
#include <stdlib.h>

static int write_file(const char *dir, const char *name, const char *ext, const void *data, size_t len)
{
    char path[4096];
    FILE *f;
    snprintf(path, sizeof(path), "%s/%s%s", dir, name, ext);
    f = fopen(path, "wb");
    if (!f)
    {
        fprintf(stderr, "cannot open %s\n", path);
        return 0;
    }
    if (len && fwrite(data, 1, len, f) != len)
    {
        fprintf(stderr, "cannot write %s\n", path);
        fclose(f);
        return 0;
    }
    return fclose(f) == 0;
}

int main(int argc, char **argv)
{
    char manifest_path[4096];
    FILE *manifest;
    size_t i;

    if (argc != 2)
    {
        fprintf(stderr, "usage: %s <output-dir>\n", argv[0]);
        return 2;
    }
    snprintf(manifest_path, sizeof(manifest_path), "%s/manifest.txt", argv[1]);
    manifest = fopen(manifest_path, "w");
    if (!manifest)
    {
        fprintf(stderr, "cannot open %s\n", manifest_path);
        return 1;
    }

    for (i = 0; i < g_png_num_test_cases; ++i)
    {
        const png_test_case *tc = &g_png_test_cases[i];
        size_t img_len = (size_t)tc->w * tc->h * tc->chans, png_len = 0;
        unsigned char *img = (unsigned char *)malloc(img_len);
        unsigned char plte[256 * 3], trns[256];
        void *png;
        int ok;

        if (!img)
            return 1;
        png_tc_fill_image(tc, img);
        png_tc_fill_palette(tc, plte, trns);
        png = tdefl_write_image_to_png_file_in_memory_ex2(img, tc->w, tc->h, tc->chans, &png_len, (mz_uint)tc->level, tc->flip,
                                                          tc->plte_entries ? plte : NULL, tc->plte_entries * 3,
                                                          tc->trns_entries ? trns : NULL, tc->trns_entries);
        if (!png)
        {
            fprintf(stderr, "%s: PNG writer failed\n", tc->name);
            return 1;
        }
        ok = write_file(argv[1], tc->name, ".png", png, png_len) &&
             write_file(argv[1], tc->name, ".raw", img, img_len) &&
             (!tc->plte_entries || write_file(argv[1], tc->name, ".plte", plte, (size_t)tc->plte_entries * 3)) &&
             (!tc->trns_entries || write_file(argv[1], tc->name, ".trns", trns, (size_t)tc->trns_entries));
        mz_free(png);
        free(img);
        if (!ok)
            return 1;
        fprintf(manifest, "%s %d %d %d %d %d %d\n", tc->name, tc->w, tc->h, tc->chans, tc->flip, tc->plte_entries, tc->trns_entries);
    }
    return fclose(manifest) == 0 ? 0 : 1;
}
