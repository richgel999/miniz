// Copyright 2025 Google LLC
// Fuzz target for miniz ZIP writer functions
// Targets: mz_zip_writer_add_mem (0% coverage, complexity 627)
//          mz_zip_writer_add_mem_ex (0% coverage)
//          mz_zip_writer_add_mem_ex_v2 (0% coverage)
//          mz_zip_writer_finalize_archive (0% coverage)

#include "miniz.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// Fuzz target for ZIP archive creation and writing
extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
    if (size < 16) return 0; // Need enough data for meaningful test
    
    // Create a memory-backed ZIP archive
    mz_zip_archive zip_archive;
    memset(&zip_archive, 0, sizeof(zip_archive));
    
    // Initialize the archive with a growable memory buffer
    size_t initial_capacity = size * 2;
    if (!mz_zip_writer_init_heap(&zip_archive, initial_capacity, 0)) {
        return 0;
    }
    
    // Extract parameters from fuzz input
    // First byte determines compression level
    int compression_level = data[0] % 11; // 0-10 (10 = best compression)
    
    // Second byte determines flags
    mz_uint flags = data[1];
    
    // Rest of data is content to add to ZIP
    const char* filename = "fuzz_test.txt";
    const uint8_t* content = data + 2;
    size_t content_size = size - 2;
    
    // Test mz_zip_writer_add_mem - 0% coverage target
    mz_bool add_result = mz_zip_writer_add_mem(
        &zip_archive,
        filename,
        content,
        content_size,
        flags | (compression_level << MZ_ZIP_LDH_BIT_OFFS_SHL)
    );
    
    // Add multiple entries with different strategies
    if (content_size > 4) {
        // Add raw (uncompressed) entry
        mz_zip_writer_add_mem(&zip_archive, "raw.bin", content, content_size / 2, 0);
        
        // Add compressed entry with store-only flag
        mz_zip_writer_add_mem(
            &zip_archive, 
            "stored.bin", 
            content + content_size / 2, 
            content_size / 2, 
            MZ_ZIP_FLAG_STORE
        );
        
        // Add with AES encryption flag (may fail without proper setup, but good for fuzzing)
        if (content_size > 8) {
            mz_zip_writer_add_mem(
                &zip_archive,
                "encrypted.bin",
                content + 4,
                content_size - 4,
                MZ_ZIP_FLAG_ENCRYPTED
            );
        }
    }
    
    // Test mz_zip_writer_finalize_archive - 0% coverage target
    mz_zip_writer_finalize_archive(&zip_archive);
    
    // Get the final archive data
    void* archive_data = NULL;
    size_t archive_size = 0;
    mz_zip_writer_finalize_heap_archive(&zip_archive, &archive_data, &archive_size);
    
    // If we successfully created an archive, try to validate it
    if (archive_data && archive_size > 0) {
        mz_zip_error zip_error = MZ_ZIP_NO_ERROR;
        mz_zip_validate_mem_archive(archive_data, archive_size, 0, &zip_error);
        
        // Cleanup
        free(archive_data);
    }
    
    // Cleanup the writer
    mz_zip_writer_end(&zip_archive);
    
    return 0;
}
