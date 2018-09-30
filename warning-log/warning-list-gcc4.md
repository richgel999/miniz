````
In file included from /miniz/miniz.h:116:0,
                 from /miniz/miniz.c:27:
/miniz/miniz_tdef.h:183:1: warning: function declaration isn't a prototype [-Wstrict-prototypes]
In file included from /miniz/miniz.h:117:0,
                 from /miniz/miniz.c:27:
/miniz/miniz_tinfl.h:48:1: warning: function declaration isn't a prototype [-Wstrict-prototypes]
/miniz/miniz.c: In function 'mz_deflateInit2':
/miniz/miniz.c:215:5: warning: conversion to 'int' from 'mz_uint' may change the sign of the result [-Wsign-conversion]
/miniz/miniz.c: In function 'mz_deflateReset':
/miniz/miniz.c:229:100: warning: conversion to 'int' from 'mz_uint' may change the sign of the result [-Wsign-conversion]
[ 11%] Building C object CMakeFiles/miniz.dir/miniz_zip.c.o
In file included from /miniz/miniz.h:116:0,
                 from /miniz/miniz_zip.h:3,
                 from /miniz/miniz_zip.c:27:
/miniz/miniz_tdef.h:183:1: warning: function declaration isn't a prototype [-Wstrict-prototypes]
In file included from /miniz/miniz.h:117:0,
                 from /miniz/miniz_zip.h:3,
                 from /miniz/miniz_zip.c:27:
/miniz/miniz_tinfl.h:48:1: warning: function declaration isn't a prototype [-Wstrict-prototypes]
/miniz/miniz_zip.c:132:9: note: #pragma message: Using fopen, ftello, fseeko, stat() etc. path for file I/O - this path may not support large files.
/miniz/miniz_zip.c: In function 'mz_zip_reader_filename_less':
/miniz/miniz_zip.c:476:18: warning: conversion to 'mz_uint8' from 'int' may alter its value [-Wconversion]
/miniz/miniz_zip.c:476:43: warning: conversion to 'mz_uint8' from 'int' may alter its value [-Wconversion]
/miniz/miniz_zip.c: In function 'mz_zip_reader_locate_header_sig':
/miniz/miniz_zip.c:560:9: warning: conversion to 'long long unsigned int' from 'mz_int64' may change the sign of the result [-Wsign-conversion]
/miniz/miniz_zip.c:560:9: warning: conversion to 'long long unsigned int' from 'mz_int64' may change the sign of the result [-Wsign-conversion]
/miniz/miniz_zip.c:562:9: warning: conversion to 'mz_uint64' from 'mz_int64' may change the sign of the result [-Wsign-conversion]
/miniz/miniz_zip.c:562:9: warning: conversion to 'size_t' from 'int' may change the sign of the result [-Wsign-conversion]
/miniz/miniz_zip.c:570:59: warning: conversion to 'long long unsigned int' from 'mz_int64' may change the sign of the result [-Wsign-conversion]
/miniz/miniz_zip.c:582:9: warning: conversion to 'long long unsigned int' from 'mz_int64' may change the sign of the result [-Wsign-conversion]
/miniz/miniz_zip.c: In function 'mz_zip_reader_read_central_dir':
/miniz/miniz_zip.c:618:5: warning: conversion to 'mz_uint64' from 'mz_int64' may change the sign of the result [-Wsign-conversion]
/miniz/miniz_zip.c:626:60: warning: conversion to 'mz_uint64' from 'mz_int64' may change the sign of the result [-Wsign-conversion]
/miniz/miniz_zip.c: In function 'mz_zip_file_read_func':
/miniz/miniz_zip.c:943:112: warning: conversion to '__off_t' from 'long long int' may alter its value [-Wconversion]
/miniz/miniz_zip.c: In function 'mz_zip_reader_init_file_v2':
/miniz/miniz_zip.c:975:31: warning: conversion to 'mz_uint64' from '__off_t' may change the sign of the result [-Wsign-conversion]
/miniz/miniz_zip.c: In function 'mz_zip_reader_init_cfile':
/miniz/miniz_zip.c:1015:30: warning: conversion to 'mz_uint64' from '__off_t' may change the sign of the result [-Wsign-conversion]
/miniz/miniz_zip.c:1022:34: warning: conversion to 'mz_uint64' from '__off_t' may change the sign of the result [-Wsign-conversion]
/miniz/miniz_zip.c: In function 'mz_zip_file_stat_internal':
/miniz/miniz_zip.c:1153:32: warning: conversion to 'mz_uint16' from 'unsigned int' may alter its value [-Wconversion]
/miniz/miniz_zip.c:1154:31: warning: conversion to 'mz_uint16' from 'unsigned int' may alter its value [-Wconversion]
/miniz/miniz_zip.c:1155:25: warning: conversion to 'mz_uint16' from 'unsigned int' may alter its value [-Wconversion]
/miniz/miniz_zip.c:1156:23: warning: conversion to 'mz_uint16' from 'unsigned int' may alter its value [-Wconversion]
/miniz/miniz_zip.c:1158:42: warning: conversion to 'int' from 'unsigned int' may change the sign of the result [-Wsign-conversion]
/miniz/miniz_zip.c:1158:86: warning: conversion to 'int' from 'unsigned int' may change the sign of the result [-Wsign-conversion]
/miniz/miniz_zip.c:1163:30: warning: conversion to 'mz_uint16' from 'unsigned int' may alter its value [-Wconversion]
/miniz/miniz_zip.c: In function 'mz_zip_filename_compare':
/miniz/miniz_zip.c:1279:18: warning: conversion to 'mz_uint8' from 'int' may alter its value [-Wconversion]
/miniz/miniz_zip.c:1279:43: warning: conversion to 'mz_uint8' from 'int' may alter its value [-Wconversion]
/miniz/miniz_zip.c: In function 'mz_zip_reader_locate_file_v2':
/miniz/miniz_zip.c:1380:36: warning: conversion to 'int' from 'mz_uint' may change the sign of the result [-Wsign-conversion]
/miniz/miniz_zip.c:1388:13: warning: conversion to 'mz_uint' from 'int' may change the sign of the result [-Wsign-conversion]
/miniz/miniz_zip.c: In function 'mz_zip_reader_extract_to_callback':
/miniz/miniz_zip.c:1745:71: warning: conversion to 'size_t' from 'mz_uint64' may alter its value [-Wconversion]
/miniz/miniz_zip.c: In function 'mz_zip_reader_extract_iter_read':
/miniz/miniz_zip.c:1974:28: warning: conversion to 'size_t' from 'mz_uint64' may alter its value [-Wconversion]
/miniz/miniz_zip.c:2014:67: warning: conversion to 'size_t' from 'mz_uint64' may alter its value [-Wconversion]
/miniz/miniz_zip.c: In function 'mz_zip_file_write_func':
/miniz/miniz_zip.c:2751:112: warning: conversion to '__off_t' from 'long long int' may alter its value [-Wconversion]
/miniz/miniz_zip.c: In function 'mz_zip_writer_init_cfile':
/miniz/miniz_zip.c:2826:58: warning: conversion to 'mz_uint64' from '__off_t' may change the sign of the result [-Wsign-conversion]
/miniz/miniz_zip.c: In function 'mz_zip_writer_add_put_buf_callback':
/miniz/miniz_zip.c:2941:5: warning: conversion to 'size_t' from 'int' may change the sign of the result [-Wsign-conversion]
/miniz/miniz_zip.c:2944:5: warning: conversion to 'mz_uint64' from 'int' may change the sign of the result [-Wsign-conversion]
/miniz/miniz_zip.c:2945:5: warning: conversion to 'mz_uint64' from 'int' may change the sign of the result [-Wsign-conversion]
/miniz/miniz_zip.c: In function 'mz_zip_writer_add_to_central_dir':
/miniz/miniz_zip.c:3051:102: warning: conversion to 'mz_uint16' from 'mz_uint' may alter its value [-Wconversion]
/miniz/miniz_zip.c: In function 'mz_zip_writer_add_mem_ex_v2':
/miniz/miniz_zip.c:3272:117: warning: conversion to 'mz_uint16' from 'mz_uint32' may alter its value [-Wconversion]
/miniz/miniz_zip.c:3299:9: warning: conversion to 'mz_uint16' from 'mz_uint' may alter its value [-Wconversion]
/miniz/miniz_zip.c:3342:9: warning: conversion to 'int' from 'mz_uint' may change the sign of the result [-Wsign-conversion]
/miniz/miniz_zip.c:3342:115: warning: conversion to 'int' from 'mz_uint' may change the sign of the result [-Wsign-conversion]
/miniz/miniz_zip.c:3394:43: warning: conversion to 'mz_uint16' from 'mz_uint32' may alter its value [-Wconversion]
/miniz/miniz_zip.c: In function 'mz_zip_writer_add_cfile':
/miniz/miniz_zip.c:3519:117: warning: conversion to 'mz_uint16' from 'mz_uint32' may alter its value [-Wconversion]
/miniz/miniz_zip.c:3543:9: warning: conversion to 'mz_uint16' from 'mz_uint' may alter its value [-Wconversion]
/miniz/miniz_zip.c:3607:13: warning: conversion to 'int' from 'mz_uint' may change the sign of the result [-Wsign-conversion]
/miniz/miniz_zip.c:3607:118: warning: conversion to 'int' from 'mz_uint' may change the sign of the result [-Wsign-conversion]
/miniz/miniz_zip.c:3695:43: warning: conversion to 'mz_uint16' from 'mz_uint32' may alter its value [-Wconversion]
/miniz/miniz_zip.c: In function 'mz_zip_writer_add_file':
/miniz/miniz_zip.c:3725:29: warning: conversion to 'mz_uint64' from '__off_t' may change the sign of the result [-Wsign-conversion]
/miniz/miniz_zip.c: In function 'mz_zip_writer_update_zip64_extension_block':
/miniz/miniz_zip.c:3776:76: warning: conversion to 'unsigned int' from 'int' may change the sign of the result [-Wsign-conversion]
/miniz/miniz_zip.c:3778:73: warning: conversion to 'size_t' from 'int' may change the sign of the result [-Wsign-conversion]
/miniz/miniz_zip.c: In function 'mz_zip_read_archive_data':
/miniz/miniz_zip.c:4615:32: warning: conversion to 'size_t' from 'mz_bool' may change the sign of the result [-Wsign-conversion]
[ 17%] Building C object CMakeFiles/miniz.dir/miniz_tinfl.c.o
In file included from /miniz/miniz_tinfl.c:27:0:
/miniz/miniz_tinfl.h:48:1: warning: function declaration isn't a prototype [-Wstrict-prototypes]
/miniz/miniz_tinfl.c: In function 'tinfl_decompress':
/miniz/miniz_tinfl.c:188:121: warning: conversion to 'size_t' from 'int' may change the sign of the result [-Wsign-conversion]
/miniz/miniz_tinfl.c:234:48: warning: conversion to 'mz_uint32' from 'int' may change the sign of the result [-Wsign-conversion]
/miniz/miniz_tinfl.c:292:21: warning: conversion to 'tinfl_bit_buf_t' from 'int' may change the sign of the result [-Wsign-conversion]
/miniz/miniz_tinfl.c:292:21: warning: conversion to 'mz_uint32' from 'char' may change the sign of the result [-Wsign-conversion]
/miniz/miniz_tinfl.c:293:67: warning: conversion to 'mz_uint32' from 'int' may change the sign of the result [-Wsign-conversion]
/miniz/miniz_tinfl.c:340:44: warning: conversion to 'mz_uint' from 'int' may change the sign of the result [-Wsign-conversion]
/miniz/miniz_tinfl.c:353:25: warning: conversion to 'mz_uint' from 'int' may change the sign of the result [-Wsign-conversion]
/miniz/miniz_tinfl.c:353:34: warning: conversion to 'int' from 'mz_uint' may change the sign of the result [-Wsign-conversion]
/miniz/miniz_tinfl.c:363:21: warning: conversion to 'mz_uint' from 'int' may change the sign of the result [-Wsign-conversion]
/miniz/miniz_tinfl.c:363:30: warning: conversion to 'int' from 'mz_uint' may change the sign of the result [-Wsign-conversion]
/miniz/miniz_tinfl.c:371:25: warning: conversion to 'mz_uint' from 'int' may change the sign of the result [-Wsign-conversion]
/miniz/miniz_tinfl.c:371:25: warning: conversion to 'tinfl_bit_buf_t' from 'int' may change the sign of the result [-Wsign-conversion]
/miniz/miniz_tinfl.c:371:25: warning: conversion to 'mz_uint' from 'int' may change the sign of the result [-Wsign-conversion]
/miniz/miniz_tinfl.c:371:25: warning: conversion to 'tinfl_bit_buf_t' from 'int' may change the sign of the result [-Wsign-conversion]
/miniz/miniz_tinfl.c:371:25: warning: conversion to 'mz_uint32' from 'int' may change the sign of the result [-Wsign-conversion]
/miniz/miniz_tinfl.c:381:48: warning: conversion to 'mz_uint32' from 'char' may change the sign of the result [-Wsign-conversion]
/miniz/miniz_tinfl.c:382:25: warning: conversion to 'tinfl_bit_buf_t' from 'int' may change the sign of the result [-Wsign-conversion]
/miniz/miniz_tinfl.c:383:25: warning: conversion to 'mz_uint' from 'char' may change the sign of the result [-Wsign-conversion]
/miniz/miniz_tinfl.c:402:25: warning: conversion to 'mz_uint' from 'int' may change the sign of the result [-Wsign-conversion]
/miniz/miniz_tinfl.c:402:25: warning: conversion to 'tinfl_bit_buf_t' from 'int' may change the sign of the result [-Wsign-conversion]
/miniz/miniz_tinfl.c:402:25: warning: conversion to 'mz_uint' from 'int' may change the sign of the result [-Wsign-conversion]
/miniz/miniz_tinfl.c:402:25: warning: conversion to 'tinfl_bit_buf_t' from 'int' may change the sign of the result [-Wsign-conversion]
/miniz/miniz_tinfl.c:402:25: warning: conversion to 'mz_uint32' from 'int' may change the sign of the result [-Wsign-conversion]
/miniz/miniz_tinfl.c:431:45: warning: conversion to 'mz_uint' from 'int' may change the sign of the result [-Wsign-conversion]
/miniz/miniz_tinfl.c:437:62: warning: conversion to 'tinfl_bit_buf_t' from 'int' may change the sign of the result [-Wsign-conversion]
/miniz/miniz_tinfl.c:440:25: warning: conversion to 'mz_uint32' from 'int' may change the sign of the result [-Wsign-conversion]
/miniz/miniz_tinfl.c:455:45: warning: conversion to 'mz_uint' from 'int' may change the sign of the result [-Wsign-conversion]
/miniz/miniz_tinfl.c:461:62: warning: conversion to 'tinfl_bit_buf_t' from 'int' may change the sign of the result [-Wsign-conversion]
/miniz/miniz_tinfl.c:471:29: warning: conversion to 'mz_uint32' from 'int' may change the sign of the result [-Wsign-conversion]
/miniz/miniz_tinfl.c:481:43: warning: conversion to 'mz_uint32' from 'int' may change the sign of the result [-Wsign-conversion]
/miniz/miniz_tinfl.c:482:40: warning: conversion to 'mz_uint32' from 'int' may change the sign of the result [-Wsign-conversion]
/miniz/miniz_tinfl.c:486:21: warning: conversion to 'tinfl_bit_buf_t' from 'int' may change the sign of the result [-Wsign-conversion]
/miniz/miniz_tinfl.c:490:17: warning: conversion to 'mz_uint' from 'int' may change the sign of the result [-Wsign-conversion]
/miniz/miniz_tinfl.c:490:17: warning: conversion to 'tinfl_bit_buf_t' from 'int' may change the sign of the result [-Wsign-conversion]
/miniz/miniz_tinfl.c:490:17: warning: conversion to 'mz_uint' from 'int' may change the sign of the result [-Wsign-conversion]
/miniz/miniz_tinfl.c:490:17: warning: conversion to 'tinfl_bit_buf_t' from 'int' may change the sign of the result [-Wsign-conversion]
/miniz/miniz_tinfl.c:490:17: warning: conversion to 'mz_uint32' from 'int' may change the sign of the result [-Wsign-conversion]
/miniz/miniz_tinfl.c:491:41: warning: conversion to 'mz_uint32' from 'int' may change the sign of the result [-Wsign-conversion]
/miniz/miniz_tinfl.c:492:35: warning: conversion to 'mz_uint32' from 'int' may change the sign of the result [-Wsign-conversion]
/miniz/miniz_tinfl.c:496:21: warning: conversion to 'tinfl_bit_buf_t' from 'int' may change the sign of the result [-Wsign-conversion]
/miniz/miniz_tinfl.c:500:56: warning: conversion to 'size_t' from 'int' may change the sign of the result [-Wsign-conversion]
/miniz/miniz_tinfl.c:203:5: warning: switch missing default case [-Wswitch-default]
/miniz/miniz_tinfl.c:608:33: warning: conversion to 'size_t' from 'int' may change the sign of the result [-Wsign-conversion]
/miniz/miniz_tinfl.c:609:35: warning: conversion to 'size_t' from 'int' may change the sign of the result [-Wsign-conversion]
/miniz/miniz_tinfl.c: In function 'tinfl_decompress_mem_to_heap':
/miniz/miniz_tinfl.c:654:85: warning: conversion to 'mz_uint32' from 'int' may change the sign of the result [-Wsign-conversion]
/miniz/miniz_tinfl.c: In function 'tinfl_decompress_mem_to_mem':
/miniz/miniz_tinfl.c:686:177: warning: conversion to 'mz_uint32' from 'int' may change the sign of the result [-Wsign-conversion]
/miniz/miniz_tinfl.c: In function 'tinfl_decompress_mem_to_callback':
/miniz/miniz_tinfl.c:703:55: warning: conversion to 'mz_uint32' from 'int' may change the sign of the result [-Wsign-conversion]
/miniz/miniz_tinfl.c: At top level:
/miniz/miniz_tinfl.c:719:21: warning: function declaration isn't a prototype [-Wstrict-prototypes]
[ 23%] Building C object CMakeFiles/miniz.dir/miniz_tdef.c.o
In file included from /miniz/miniz_tdef.c:27:0:
/miniz/miniz_tdef.h:183:1: warning: function declaration isn't a prototype [-Wstrict-prototypes]
In file included from /miniz/miniz.h:117:0,
                 from /miniz/miniz_tdef.c:28:
/miniz/miniz_tinfl.h:48:1: warning: function declaration isn't a prototype [-Wstrict-prototypes]
/miniz/miniz_tdef.c: In function 'tdefl_calculate_minimum_redundancy':
/miniz/miniz_tdef.c:148:16: warning: conversion to 'mz_uint16' from 'int' may alter its value [-Wconversion]
/miniz/miniz_tdef.c:170:48: warning: conversion to 'mz_uint16' from 'int' may alter its value [-Wconversion]
/miniz/miniz_tdef.c: In function 'tdefl_optimize_huffman_table':
/miniz/miniz_tdef.c:244:9: warning: conversion to 'mz_uint' from 'int' may change the sign of the result [-Wsign-conversion]
/miniz/miniz_tdef.c:261:26: warning: conversion to 'mz_uint' from 'int' may change the sign of the result [-Wsign-conversion]
/miniz/miniz_tdef.c:269:9: warning: conversion to 'int' from 'mz_uint' may change the sign of the result [-Wsign-conversion]
/miniz/miniz_tdef.c: In function 'tdefl_start_dynamic_block':
/miniz/miniz_tdef.c:359:5: warning: conversion to 'size_t' from 'int' may change the sign of the result [-Wsign-conversion]
/miniz/miniz_tdef.c:360:5: warning: conversion to 'size_t' from 'int' may change the sign of the result [-Wsign-conversion]
/miniz/miniz_tdef.c:361:46: warning: conversion to 'mz_uint' from 'int' may change the sign of the result [-Wsign-conversion]
/miniz/miniz_tdef.c:407:5: warning: conversion to 'mz_uint' from 'int' may change the sign of the result [-Wsign-conversion]
/miniz/miniz_tdef.c:408:5: warning: conversion to 'mz_uint' from 'int' may change the sign of the result [-Wsign-conversion]
/miniz/miniz_tdef.c:414:5: warning: conversion to 'mz_uint' from 'int' may change the sign of the result [-Wsign-conversion]
/miniz/miniz_tdef.c:424:13: warning: conversion to 'mz_uint' from 'char' may change the sign of the result [-Wsign-conversion]
/miniz/miniz_tdef.c: In function 'tdefl_compress_lz_codes':
/miniz/miniz_tdef.c:556:34: warning: conversion to 'mz_uint' from 'int' may change the sign of the result [-Wsign-conversion]
/miniz/miniz_tdef.c:560:74: warning: conversion to 'mz_uint' from 'int' may change the sign of the result [-Wsign-conversion]
/miniz/miniz_tdef.c: In function 'tdefl_flush_block':
/miniz/miniz_tdef.c:638:74: warning: conversion to 'unsigned int' from 'int' may change the sign of the result [-Wsign-conversion]
/miniz/miniz_tdef.c:715:44: warning: conversion to 'size_t' from 'int' may change the sign of the result [-Wsign-conversion]
/miniz/miniz_tdef.c:722:13: warning: conversion to 'size_t' from 'int' may change the sign of the result [-Wsign-conversion]
/miniz/miniz_tdef.c:723:13: warning: conversion to 'size_t' from 'int' may change the sign of the result [-Wsign-conversion]
/miniz/miniz_tdef.c:726:17: warning: conversion to 'mz_uint' from 'int' may change the sign of the result [-Wsign-conversion]
/miniz/miniz_tdef.c:727:17: warning: conversion to 'mz_uint' from 'int' may change the sign of the result [-Wsign-conversion]
/miniz/miniz_tdef.c:732:13: warning: conversion to 'size_t' from 'int' may change the sign of the result [-Wsign-conversion]
/miniz/miniz_tdef.c:736:13: warning: conversion to 'int' from 'mz_uint' may change the sign of the result [-Wsign-conversion]
/miniz/miniz_tdef.c: In function 'tdefl_compress_normal':
/miniz/miniz_tdef.c:1082:98: warning: conversion to 'mz_uint' from 'int' may change the sign of the result [-Wsign-conversion]
/miniz/miniz_tdef.c: In function 'tdefl_flush_output_buffer':
/miniz/miniz_tdef.c:1218:40: warning: conversion to 'size_t' from 'int' may change the sign of the result [-Wsign-conversion]
/miniz/miniz_tdef.c: In function 'tdefl_compress':
/miniz/miniz_tdef.c:1285:97: warning: conversion to 'size_t' from 'int' may change the sign of the result [-Wsign-conversion]
/miniz/miniz_tdef.c: In function 'tdefl_init':
/miniz/miniz_tdef.c:1314:28: warning: conversion to 'mz_uint' from 'int' may change the sign of the result [-Wsign-conversion]
/miniz/miniz_tdef.c:1316:28: warning: conversion to 'mz_uint' from 'int' may change the sign of the result [-Wsign-conversion]
/miniz/miniz_tdef.c: In function 'tdefl_output_buffer_putter':
/miniz/miniz_tdef.c:1379:5: warning: conversion to 'size_t' from 'int' may change the sign of the result [-Wsign-conversion]
/miniz/miniz_tdef.c:1396:5: warning: conversion to 'size_t' from 'int' may change the sign of the result [-Wsign-conversion]
/miniz/miniz_tdef.c: In function 'tdefl_create_comp_flags_from_zip_params':
/miniz/miniz_tdef.c:1443:9: warning: negative integer implicitly converted to unsigned type [-Wsign-conversion]
/miniz/miniz_tdef.c: In function 'tdefl_write_image_to_png_file_in_memory_ex':
/miniz/miniz_tdef.c:1473:29: warning: conversion to 'size_t' from 'int' may change the sign of the result [-Wsign-conversion]
/miniz/miniz_tdef.c:1483:103: warning: conversion to 'int' from 'unsigned int' may change the sign of the result [-Wsign-conversion]
/miniz/miniz_tdef.c:1487:9: warning: conversion to 'size_t' from 'int' may change the sign of the result [-Wsign-conversion]
/miniz/miniz_tdef.c: At top level:
/miniz/miniz_tdef.c:1547:19: warning: function declaration isn't a prototype [-Wstrict-prototypes]
[ 29%] Linking C static library libminiz.a
[ 29%] Built target miniz
Scanning dependencies of target example1
[ 35%] Building C object CMakeFiles/example1.dir/examples/example1.c.o
/miniz/examples/example1.c:1:1: warning: C++ style comments are not allowed in ISO C90 [enabled by default]
/miniz/examples/example1.c:1:1: warning: (this will be reported only once per input file) [enabled by default]
In file included from /miniz/miniz.h:116:0,
                 from /miniz/examples/example1.c:4:
/miniz/miniz_tdef.h:183:1: warning: function declaration isn't a prototype [-Wstrict-prototypes]
In file included from /miniz/miniz.h:117:0,
                 from /miniz/examples/example1.c:4:
/miniz/miniz_tinfl.h:48:1: warning: function declaration isn't a prototype [-Wstrict-prototypes]
/miniz/examples/example1.c: In function 'main':
/miniz/examples/example1.c:57:18: warning: conversion to 'unsigned int' from 'int' may change the sign of the result [-Wsign-conversion]
/miniz/examples/example1.c:60:22: warning: conversion to 'long unsigned int' from 'int' may change the sign of the result [-Wsign-conversion]
/miniz/examples/example1.c:61:17: warning: conversion to 'uint8' from 'int' may alter its value [-Wconversion]
[ 41%] Linking C executable ../bin/example1
[ 41%] Built target example1
Scanning dependencies of target example2
[ 47%] Building C object CMakeFiles/example2.dir/examples/example2.c.o
/miniz/examples/example2.c:1:1: warning: C++ style comments are not allowed in ISO C90 [enabled by default]
/miniz/examples/example2.c:1:1: warning: (this will be reported only once per input file) [enabled by default]
In file included from /miniz/miniz.h:116:0,
                 from /miniz/miniz_zip.h:3,
                 from /miniz/examples/example2.c:16:
/miniz/miniz_tdef.h:183:1: warning: function declaration isn't a prototype [-Wstrict-prototypes]
In file included from /miniz/miniz.h:117:0,
                 from /miniz/miniz_zip.h:3,
                 from /miniz/examples/example2.c:16:
/miniz/miniz_tinfl.h:48:1: warning: function declaration isn't a prototype [-Wstrict-prototypes]
/miniz/examples/example2.c:24:3: warning: string length '965' is greater than the length '509' ISO C90 compilers are required to support [-Woverlength-strings]
/miniz/examples/example2.c: In function 'main':
/miniz/examples/example2.c:94:5: warning: conversion to 'mz_uint' from 'int' may change the sign of the result [-Wsign-conversion]
/miniz/examples/example2.c:101:5: warning: conversion to 'mz_uint' from 'int' may change the sign of the result [-Wsign-conversion]
/miniz/examples/example2.c:105:7: warning: conversion to 'mz_uint' from 'int' may change the sign of the result [-Wsign-conversion]
[ 52%] Linking C executable ../bin/example2
[ 52%] Built target example2
Scanning dependencies of target example3
[ 58%] Building C object CMakeFiles/example3.dir/examples/example3.c.o
/miniz/examples/example3.c:1:1: warning: C++ style comments are not allowed in ISO C90 [enabled by default]
/miniz/examples/example3.c:1:1: warning: (this will be reported only once per input file) [enabled by default]
In file included from /miniz/miniz.h:116:0,
                 from /miniz/examples/example3.c:6:
/miniz/miniz_tdef.h:183:1: warning: function declaration isn't a prototype [-Wstrict-prototypes]
In file included from /miniz/miniz.h:117:0,
                 from /miniz/examples/example3.c:6:
/miniz/miniz_tinfl.h:48:1: warning: function declaration isn't a prototype [-Wstrict-prototypes]
[ 64%] Linking C executable ../bin/example3
[ 64%] Built target example3
Scanning dependencies of target example4
[ 70%] Building C object CMakeFiles/example4.dir/examples/example4.c.o
/miniz/examples/example4.c:1:1: warning: C++ style comments are not allowed in ISO C90 [enabled by default]
/miniz/examples/example4.c:1:1: warning: (this will be reported only once per input file) [enabled by default]
In file included from /miniz/examples/example4.c:3:0:
/miniz/miniz_tinfl.h:48:1: warning: function declaration isn't a prototype [-Wstrict-prototypes]
/miniz/examples/example4.c: In function 'tinfl_put_buf_func':
/miniz/examples/example4.c:16:3: warning: conversion to 'size_t' from 'int' may change the sign of the result [-Wsign-conversion]
/miniz/examples/example4.c: In function 'main':
/miniz/examples/example4.c:89:23: warning: conversion to 'unsigned int' from 'long int' may change the sign of the result [-Wsign-conversion]
[ 76%] Linking C executable ../bin/example4
[ 76%] Built target example4
Scanning dependencies of target example5
[ 82%] Building C object CMakeFiles/example5.dir/examples/example5.c.o
/miniz/examples/example5.c:1:1: warning: C++ style comments are not allowed in ISO C90 [enabled by default]
/miniz/examples/example5.c:1:1: warning: (this will be reported only once per input file) [enabled by default]
In file included from /miniz/miniz.h:116:0,
                 from /miniz/examples/example5.c:12:
/miniz/miniz_tdef.h:183:1: warning: function declaration isn't a prototype [-Wstrict-prototypes]
In file included from /miniz/miniz.h:117:0,
                 from /miniz/examples/example5.c:12:
/miniz/miniz_tinfl.h:48:1: warning: function declaration isn't a prototype [-Wstrict-prototypes]
/miniz/examples/example5.c: In function 'main':
/miniz/examples/example5.c:168:7: warning: conversion to 'int' from 'mz_uint' may change the sign of the result [-Wsign-conversion]
[ 88%] Linking C executable ../bin/example5
[ 88%] Built target example5
Scanning dependencies of target example6
[ 94%] Building C object CMakeFiles/example6.dir/examples/example6.c.o
/miniz/examples/example6.c:1:1: warning: C++ style comments are not allowed in ISO C90 [enabled by default]
/miniz/examples/example6.c:1:1: warning: (this will be reported only once per input file) [enabled by default]
In file included from /miniz/miniz.h:116:0,
                 from /miniz/examples/example6.c:10:
/miniz/miniz_tdef.h:183:1: warning: function declaration isn't a prototype [-Wstrict-prototypes]
In file included from /miniz/miniz.h:117:0,
                 from /miniz/examples/example6.c:10:
/miniz/miniz_tinfl.h:48:1: warning: function declaration isn't a prototype [-Wstrict-prototypes]
/miniz/examples/example6.c: In function 'hsv_to_rgb':
/miniz/examples/example6.c:35:44: warning: conversion to 'uint8' from 'int' may alter its value [-Wconversion]
/miniz/examples/example6.c:38:3: warning: ISO C90 forbids mixed declarations and code [-pedantic]
/miniz/examples/example6.c:39:3: warning: conversion to 'float' from 'int' may alter its value [-Wconversion]
/miniz/examples/example6.c:45:3: warning: conversion to 'uint8' from 'double' may alter its value [-Wconversion]
/miniz/examples/example6.c:45:3: warning: conversion to 'uint8' from 'double' may alter its value [-Wconversion]
/miniz/examples/example6.c:46:3: warning: conversion to 'uint8' from 'double' may alter its value [-Wconversion]
/miniz/examples/example6.c:46:3: warning: conversion to 'uint8' from 'double' may alter its value [-Wconversion]
/miniz/examples/example6.c:47:3: warning: conversion to 'uint8' from 'double' may alter its value [-Wconversion]
/miniz/examples/example6.c:47:3: warning: conversion to 'uint8' from 'double' may alter its value [-Wconversion]
/miniz/examples/example6.c:48:3: warning: conversion to 'uint8' from 'double' may alter its value [-Wconversion]
/miniz/examples/example6.c:48:3: warning: conversion to 'uint8' from 'double' may alter its value [-Wconversion]
/miniz/examples/example6.c:49:3: warning: conversion to 'uint8' from 'double' may alter its value [-Wconversion]
/miniz/examples/example6.c:49:3: warning: conversion to 'uint8' from 'double' may alter its value [-Wconversion]
/miniz/examples/example6.c:50:3: warning: conversion to 'uint8' from 'double' may alter its value [-Wconversion]
/miniz/examples/example6.c:50:3: warning: conversion to 'uint8' from 'double' may alter its value [-Wconversion]
/miniz/examples/example6.c: In function 'main':
/miniz/examples/example6.c:59:3: warning: ISO C90 forbids mixed declarations and code [-pedantic]
/miniz/examples/example6.c:85:45: warning: conversion to 'size_t' from 'int' may change the sign of the result [-Wsign-conversion]
/miniz/examples/example6.c:119:35: warning: conversion to 'uint8' from 'int' may alter its value [-Wconversion]
/miniz/examples/example6.c:135:34: warning: conversion to 'unsigned int' from 'int' may change the sign of the result [-Wsign-conversion]
/miniz/examples/example6.c:137:7: warning: conversion to 'int' from 'unsigned int' may change the sign of the result [-Wsign-conversion]
[100%] Linking C executable ../bin/example6
[100%] Built target example6

````
