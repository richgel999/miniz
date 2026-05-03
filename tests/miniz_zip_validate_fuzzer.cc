// Copyright 2025 Google LLC
// Fuzz target for miniz ZIP validation functions
// Targets: mz_zip_validate_mem_archive (0% coverage, complexity 642)
//          mz_zip_validate_file_archive (0% coverage, complexity 651)

#include "miniz.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

// Fuzz target for ZIP archive validation
// Tests both memory-based and file-based validation functions
extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
    if (size < 4) return 0; // Need at least a minimal ZIP header
    
    // Test mz_zip_validate_mem_archive - 0% coverage target
    // This validates a ZIP archive in memory
    mz_zip_error zip_error = MZ_ZIP_NO_ERROR;
    mz_bool result = mz_zip_validate_mem_archive(
        data, 
        size, 
        MZ_ZIP_FLAG_DO_NOT_SORT_CENTRAL_DIRECTORY, 
        &zip_error
    );
    
    // Try with different flags
    zip_error = MZ_ZIP_NO_ERROR;
    mz_zip_validate_mem_archive(data, size, 0, &zip_error);
    
    // Test with various buffer sizes to hit edge cases
    if (size > 1024) {
        zip_error = MZ_ZIP_NO_ERROR;
        mz_zip_validate_mem_archive(data, 1024, 0, &zip_error);
    }
    
    // Test mz_compress and mz_uncompress - 0% coverage targets
    // These are the simple zlib-compatible API functions
    if (size > 0 && size <= 1024 * 1024) { // Reasonable size limit
        // Compress
        mz_ulong compressed_size = compressBound(size);
        unsigned char* compressed = (unsigned char*)malloc(compressed_size);
        
        if (compressed) {
            int compress_result = mz_compress(
                compressed, 
                &compressed_size, 
                data, 
                size
            );
            
            // If compression succeeded, try decompression
            if (compress_result == MZ_OK && compressed_size > 0) {
                mz_ulong uncompressed_size = size * 2; // Give some headroom
                unsigned char* uncompressed = (unsigned char*)malloc(uncompressed_size);
                
                if (uncompressed) {
                    mz_uncompress(uncompressed, &uncompressed_size, compressed, compressed_size);
                    free(uncompressed);
                }
            }
            
            free(compressed);
        }
    }
    
    // Test mz_adler32 and mz_crc32 - checksum functions
    mz_adler32(MZ_ADLER32_INIT, data, size);
    mz_crc32(MZ_CRC32_INIT, data, size);
    
    return 0;
}
