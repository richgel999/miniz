// Copyright 2025 Google LLC
// Fuzz target for miniz tdefl (tiny deflate) functions
// Targets: tdefl_compress_mem_to_mem (0% coverage, complexity ~400)
//          tdefl_compress_mem_to_heap (0% coverage)
//          tdefl_write_image_to_png_file_in_memory (0% coverage, complexity 503)

#include "miniz.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

// Fuzz target for tdefl compression functions
extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
    if (size < 1) return 0;
    
    // Determine compression level from input
    tdefl_compress_level compression_level = (tdefl_compress_level)(data[0] % TDEFL_MAX_LEVELS);
    const uint8_t* content = data + 1;
    size_t content_size = size - 1;
    
    // Test tdefl_compress_mem_to_mem - 0% coverage target
    if (content_size > 0 && content_size <= 10 * 1024 * 1024) { // Max 10MB
        size_t max_compressed_size = content_size + 1024;
        void* compressed = malloc(max_compressed_size);
        
        if (compressed) {
            size_t actual_compressed_size = tdefl_compress_mem_to_mem(
                compressed,
                max_compressed_size,
                content,
                content_size,
                compression_level
            );
            
            // If compression succeeded, try decompression
            if (actual_compressed_size > 0 && actual_compressed_size <= max_compressed_size) {
                size_t max_uncompressed = content_size * 2;
                void* uncompressed = malloc(max_uncompressed);
                
                if (uncompressed) {
                    tinfl_decompress_mem_to_mem(
                        uncompressed,
                        max_uncompressed,
                        compressed,
                        actual_compressed_size,
                        TINFL_FLAG_PARSE_ZLIB_HEADER
                    );
                    free(uncompressed);
                }
            }
            
            free(compressed);
        }
    }
    
    // Test tdefl_compress_mem_to_heap - 0% coverage target
    if (content_size > 0 && content_size <= 10 * 1024 * 1024) {
        size_t compressed_size = 0;
        void* heap_compressed = tdefl_compress_mem_to_heap(
            content,
            content_size,
            &compressed_size,
            compression_level
        );
        
        if (heap_compressed) {
            // Decompress to verify
            if (compressed_size > 0) {
                size_t uncomp_size = content_size * 2;
                void* uncomp = malloc(uncomp_size);
                if (uncomp) {
                    tinfl_decompress_mem_to_mem(
                        uncomp,
                        uncomp_size,
                        heap_compressed,
                        compressed_size,
                        TINFL_FLAG_PARSE_ZLIB_HEADER
                    );
                    free(uncomp);
                }
            }
            free(heap_compressed);
        }
    }
    
    // Test tdefl_write_image_to_png_file_in_memory - 0% coverage target (complexity 503)
    // This function compresses a grayscale or RGB/RGBA image to PNG format
    if (content_size >= 4) {
        // Try to interpret input as a small image
        // Extract dimensions from first bytes
        int w = (data[0] % 64) + 1;  // 1-64 width
        int h = (data[1] % 64) + 1;  // 1-64 height
        int num_chans = (data[2] % 4) + 1;  // 1-4 channels (grayscale, RGB, RGBA)
        
        size_t expected_size = w * h * num_chans;
        if (content_size - 3 >= expected_size && expected_size > 0) {
            const void* image_data = data + 3;
            
            size_t png_size = 0;
            void* png_data = tdefl_write_image_to_png_file_in_memory(
                image_data,
                w,
                h,
                num_chans,
                &png_size
            );
            
            if (png_data && png_size > 0) {
                free(png_data);
            }
        }
    }
    
    return 0;
}
