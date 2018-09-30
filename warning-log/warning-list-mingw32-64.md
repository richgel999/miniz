````
/home/admin/main_daemon/miniz/miniz_tinfl.c:101:33: warning: conversion to 'tinfl_bit_buf_t' from 'int' may change the sign of the result [-Wsign-conversion]
         b = (mz_uint32)(bit_buf & ((1 << (n)) - 1));      \
                                 ^
/home/admin/main_daemon/miniz/miniz_tinfl.c:297:21: note: in expansion of macro 'TINFL_GET_BITS'
                     TINFL_GET_BITS(11, r->m_table_sizes[counter], (mz_uint8)("\05\05\04"[counter]));
                     ^
/home/admin/main_daemon/miniz/miniz_tinfl.c:117:24: warning: conversion to 'mz_uint' from 'int' may change the sign of the result [-Wsign-conversion]
             code_len = temp >> 9;                                              \
                        ^
/home/admin/main_daemon/miniz/miniz_tinfl.c:151:17: note: in expansion of macro 'TINFL_HUFF_BITBUF_FILL'
                 TINFL_HUFF_BITBUF_FILL(state_index, pHuff);                                                                         \
                 ^
/home/admin/main_daemon/miniz/miniz_tinfl.c:376:25: note: in expansion of macro 'TINFL_HUFF_DECODE'
                         TINFL_HUFF_DECODE(16, dist, &r->m_tables[2]);
                         ^
/home/admin/main_daemon/miniz/miniz_tinfl.c:126:46: warning: conversion to 'tinfl_bit_buf_t' from 'int' may change the sign of the result [-Wsign-conversion]
                 temp = (pHuff)->m_tree[~temp + ((bit_buf >> code_len++) & 1)]; \
                                              ^
/home/admin/main_daemon/miniz/miniz_tinfl.c:151:17: note: in expansion of macro 'TINFL_HUFF_BITBUF_FILL'
                 TINFL_HUFF_BITBUF_FILL(state_index, pHuff);                                                                         \
                 ^
/home/admin/main_daemon/miniz/miniz_tinfl.c:376:25: note: in expansion of macro 'TINFL_HUFF_DECODE'
                         TINFL_HUFF_DECODE(16, dist, &r->m_tables[2]);
                         ^
/home/admin/main_daemon/miniz/miniz_tinfl.c:161:24: warning: conversion to 'mz_uint' from 'int' may change the sign of the result [-Wsign-conversion]
             code_len = temp >> 9, temp &= 511;                                                                                      \
                        ^
/home/admin/main_daemon/miniz/miniz_tinfl.c:376:25: note: in expansion of macro 'TINFL_HUFF_DECODE'
                         TINFL_HUFF_DECODE(16, dist, &r->m_tables[2]);
                         ^
/home/admin/main_daemon/miniz/miniz_tinfl.c:167:46: warning: conversion to 'tinfl_bit_buf_t' from 'int' may change the sign of the result [-Wsign-conversion]
                 temp = (pHuff)->m_tree[~temp + ((bit_buf >> code_len++) & 1)];                                                      \
                                              ^
/home/admin/main_daemon/miniz/miniz_tinfl.c:376:25: note: in expansion of macro 'TINFL_HUFF_DECODE'
                         TINFL_HUFF_DECODE(16, dist, &r->m_tables[2]);
                         ^
/home/admin/main_daemon/miniz/miniz_tinfl.c:101:33: warning: conversion to 'tinfl_bit_buf_t' from 'int' may change the sign of the result [-Wsign-conversion]
         b = (mz_uint32)(bit_buf & ((1 << (n)) - 1));      \
                                 ^
/home/admin/main_daemon/miniz/miniz_tinfl.c:387:25: note: in expansion of macro 'TINFL_GET_BITS'
                         TINFL_GET_BITS(18, s, num_extra);
                         ^
/home/admin/main_daemon/miniz/miniz_tinfl.c:117:24: warning: conversion to 'mz_uint' from 'int' may change the sign of the result [-Wsign-conversion]
             code_len = temp >> 9;                                              \
                        ^
/home/admin/main_daemon/miniz/miniz_tinfl.c:151:17: note: in expansion of macro 'TINFL_HUFF_BITBUF_FILL'
                 TINFL_HUFF_BITBUF_FILL(state_index, pHuff);                                                                         \
                 ^
/home/admin/main_daemon/miniz/miniz_tinfl.c:407:25: note: in expansion of macro 'TINFL_HUFF_DECODE'
                         TINFL_HUFF_DECODE(23, counter, &r->m_tables[0]);
                         ^
/home/admin/main_daemon/miniz/miniz_tinfl.c:126:46: warning: conversion to 'tinfl_bit_buf_t' from 'int' may change the sign of the result [-Wsign-conversion]
                 temp = (pHuff)->m_tree[~temp + ((bit_buf >> code_len++) & 1)]; \
                                              ^
/home/admin/main_daemon/miniz/miniz_tinfl.c:151:17: note: in expansion of macro 'TINFL_HUFF_BITBUF_FILL'
                 TINFL_HUFF_BITBUF_FILL(state_index, pHuff);                                                                         \
                 ^
/home/admin/main_daemon/miniz/miniz_tinfl.c:407:25: note: in expansion of macro 'TINFL_HUFF_DECODE'
                         TINFL_HUFF_DECODE(23, counter, &r->m_tables[0]);
                         ^
/home/admin/main_daemon/miniz/miniz_tinfl.c:161:24: warning: conversion to 'mz_uint' from 'int' may change the sign of the result [-Wsign-conversion]
             code_len = temp >> 9, temp &= 511;                                                                                      \
                        ^
/home/admin/main_daemon/miniz/miniz_tinfl.c:407:25: note: in expansion of macro 'TINFL_HUFF_DECODE'
                         TINFL_HUFF_DECODE(23, counter, &r->m_tables[0]);
                         ^
/home/admin/main_daemon/miniz/miniz_tinfl.c:167:46: warning: conversion to 'tinfl_bit_buf_t' from 'int' may change the sign of the result [-Wsign-conversion]
                 temp = (pHuff)->m_tree[~temp + ((bit_buf >> code_len++) & 1)];                                                      \
                                              ^
/home/admin/main_daemon/miniz/miniz_tinfl.c:407:25: note: in expansion of macro 'TINFL_HUFF_DECODE'
                         TINFL_HUFF_DECODE(23, counter, &r->m_tables[0]);
                         ^
/home/admin/main_daemon/miniz/miniz_tinfl.c:442:68: warning: conversion to 'tinfl_bit_buf_t' from 'int' may change the sign of the result [-Wsign-conversion]
                                 sym2 = r->m_tables[0].m_tree[~sym2 + ((bit_buf >> code_len++) & 1)];
                                                                    ^
/home/admin/main_daemon/miniz/miniz_tinfl.c:466:68: warning: conversion to 'tinfl_bit_buf_t' from 'int' may change the sign of the result [-Wsign-conversion]
                                 sym2 = r->m_tables[0].m_tree[~sym2 + ((bit_buf >> code_len++) & 1U)];
                                                                    ^
/home/admin/main_daemon/miniz/miniz_tinfl.c:101:33: warning: conversion to 'tinfl_bit_buf_t' from 'int' may change the sign of the result [-Wsign-conversion]
         b = (mz_uint32)(bit_buf & ((1 << (n)) - 1));      \
                                 ^
/home/admin/main_daemon/miniz/miniz_tinfl.c:491:21: note: in expansion of macro 'TINFL_GET_BITS'
                     TINFL_GET_BITS(25, extra_bits, num_extra);
                     ^
/home/admin/main_daemon/miniz/miniz_tinfl.c:117:24: warning: conversion to 'mz_uint' from 'int' may change the sign of the result [-Wsign-conversion]
             code_len = temp >> 9;                                              \
                        ^
/home/admin/main_daemon/miniz/miniz_tinfl.c:151:17: note: in expansion of macro 'TINFL_HUFF_BITBUF_FILL'
                 TINFL_HUFF_BITBUF_FILL(state_index, pHuff);                                                                         \
                 ^
/home/admin/main_daemon/miniz/miniz_tinfl.c:495:17: note: in expansion of macro 'TINFL_HUFF_DECODE'
                 TINFL_HUFF_DECODE(26, dist, &r->m_tables[1]);
                 ^
/home/admin/main_daemon/miniz/miniz_tinfl.c:126:46: warning: conversion to 'tinfl_bit_buf_t' from 'int' may change the sign of the result [-Wsign-conversion]
                 temp = (pHuff)->m_tree[~temp + ((bit_buf >> code_len++) & 1)]; \
                                              ^
/home/admin/main_daemon/miniz/miniz_tinfl.c:151:17: note: in expansion of macro 'TINFL_HUFF_BITBUF_FILL'
                 TINFL_HUFF_BITBUF_FILL(state_index, pHuff);                                                                         \
                 ^
/home/admin/main_daemon/miniz/miniz_tinfl.c:495:17: note: in expansion of macro 'TINFL_HUFF_DECODE'
                 TINFL_HUFF_DECODE(26, dist, &r->m_tables[1]);
                 ^
/home/admin/main_daemon/miniz/miniz_tinfl.c:161:24: warning: conversion to 'mz_uint' from 'int' may change the sign of the result [-Wsign-conversion]
             code_len = temp >> 9, temp &= 511;                                                                                      \
                        ^
/home/admin/main_daemon/miniz/miniz_tinfl.c:495:17: note: in expansion of macro 'TINFL_HUFF_DECODE'
                 TINFL_HUFF_DECODE(26, dist, &r->m_tables[1]);
                 ^
/home/admin/main_daemon/miniz/miniz_tinfl.c:167:46: warning: conversion to 'tinfl_bit_buf_t' from 'int' may change the sign of the result [-Wsign-conversion]
                 temp = (pHuff)->m_tree[~temp + ((bit_buf >> code_len++) & 1)];                                                      \
                                              ^
/home/admin/main_daemon/miniz/miniz_tinfl.c:495:17: note: in expansion of macro 'TINFL_HUFF_DECODE'
                 TINFL_HUFF_DECODE(26, dist, &r->m_tables[1]);
                 ^
/home/admin/main_daemon/miniz/miniz_tinfl.c:101:33: warning: conversion to 'tinfl_bit_buf_t' from 'int' may change the sign of the result [-Wsign-conversion]
         b = (mz_uint32)(bit_buf & ((1 << (n)) - 1));      \
                                 ^
/home/admin/main_daemon/miniz/miniz_tinfl.c:501:21: note: in expansion of macro 'TINFL_GET_BITS'
                     TINFL_GET_BITS(27, extra_bits, num_extra);
                     ^
[ 40%] Building C object CMakeFiles/miniz.dir/miniz_tdef.c.obj
/home/admin/main_daemon/miniz/miniz_tdef.c: In function 'tdefl_create_comp_flags_from_zip_params':
/home/admin/main_daemon/miniz/miniz_tdef.c:1462:20: warning: negative integer implicitly converted to unsigned type [-Wsign-conversion]
         comp_flags &= ~TDEFL_MAX_PROBES_MASK;
                    ^
Linking C shared library ../../bin/libminiz.dll
[ 40%] Built target miniz
Scanning dependencies of target example1
[ 50%] Building C object CMakeFiles/example1.dir/examples/example1.c.obj
/home/admin/main_daemon/miniz/examples/example1.c: In function 'main':
/home/admin/main_daemon/miniz/examples/example1.c:57:16: warning: conversion to 'uint' from 'int' may change the sign of the result [-Wsign-conversion]
       uint n = 1 + (rand() % 3);
                ^
/home/admin/main_daemon/miniz/examples/example1.c:60:25: warning: conversion to 'long unsigned int' from 'int' may change the sign of the result [-Wsign-conversion]
         uint i = rand() % cmp_len;
                         ^
/home/admin/main_daemon/miniz/examples/example1.c:61:20: warning: conversion to 'uint8' from 'int' may alter its value [-Wconversion]
         pCmp[i] ^= (rand() & 0xFF);
                    ^
Linking C executable ../../bin/example1.exe
[ 50%] Built target example1
Scanning dependencies of target example2
[ 60%] Building C object CMakeFiles/example2.dir/examples/example2.c.obj
/home/admin/main_daemon/miniz/examples/example2.c: In function 'main':
/home/admin/main_daemon/miniz/examples/example2.c:94:48: warning: conversion to 'mz_uint' from 'int' may change the sign of the result [-Wsign-conversion]
     if (!mz_zip_reader_file_stat(&zip_archive, i, &file_stat))
                                                ^
/home/admin/main_daemon/miniz/examples/example2.c:101:258: warning: conversion to 'mz_uint' from 'int' may change the sign of the result [-Wsign-conversion]
     printf("Filename: \"%s\", Comment: \"%s\", Uncompressed size: %u, Compressed size: %u, Is Dir: %u\n", file_stat.m_filename, file_stat.m_comment, (uint)file_stat.m_uncomp_size, (uint)file_stat.m_comp_size, mz_zip_reader_is_file_a_directory(&zip_archive, i));
                                                                                                                                                                                                                                                                  ^
/home/admin/main_daemon/miniz/examples/example2.c:105:60: warning: conversion to 'mz_uint' from 'int' may change the sign of the result [-Wsign-conversion]
       if (!mz_zip_reader_is_file_a_directory(&zip_archive, i))
                                                            ^
Linking C executable ../../bin/example2.exe
[ 60%] Built target example2
Scanning dependencies of target example3
[ 70%] Building C object CMakeFiles/example3.dir/examples/example3.c.obj
Linking C executable ../../bin/example3.exe
[ 70%] Built target example3
Scanning dependencies of target example4
[ 80%] Building C object CMakeFiles/example4.dir/examples/example4.c.obj
/home/admin/main_daemon/miniz/examples/example4.c: In function 'tinfl_put_buf_func':
/home/admin/main_daemon/miniz/examples/example4.c:16:38: warning: conversion to 'size_t' from 'int' may change the sign of the result [-Wsign-conversion]
   return len == (int)fwrite(pBuf, 1, len, (FILE*)pUser);
                                      ^
/home/admin/main_daemon/miniz/examples/example4.c: In function 'main':
/home/admin/main_daemon/miniz/examples/example4.c:89:18: warning: conversion to 'uint' from 'long int' may change the sign of the result [-Wsign-conversion]
   outfile_size = ftell(pOutfile);
                  ^
Linking C executable ../../bin/example4.exe
[ 80%] Built target example4
Scanning dependencies of target example5
[ 90%] Building C object CMakeFiles/example5.dir/examples/example5.c.obj
/home/admin/main_daemon/miniz/examples/example5.c: In function 'main':
/home/admin/main_daemon/miniz/examples/example5.c:168:52: warning: conversion to 'int' from 'mz_uint' may change the sign of the result [-Wsign-conversion]
       status = tdefl_init(&g_deflator, NULL, NULL, comp_flags);
                                                    ^
Linking C executable ../../bin/example5.exe
[ 90%] Built target example5
Scanning dependencies of target example6
[100%] Building C object CMakeFiles/example6.dir/examples/example6.c.obj
/home/admin/main_daemon/miniz/examples/example6.c: In function 'hsv_to_rgb':
/home/admin/main_daemon/miniz/examples/example6.c:35:26: warning: conversion to 'uint8' from 'int' may alter its value [-Wconversion]
     p->r = p->g = p->b = 255 * (max - hue) / (max - min);
                          ^
/home/admin/main_daemon/miniz/examples/example6.c:39:21: warning: conversion to 'float' from 'int' may alter its value [-Wconversion]
   double c = 255.0f * saturation;
                     ^
/home/admin/main_daemon/miniz/examples/example6.c:45:18: warning: conversion to 'uint8' from 'double' may alter its value [-Wfloat-conversion]
   case 0: p->r = c; p->g = X; return;
                  ^
/home/admin/main_daemon/miniz/examples/example6.c:45:28: warning: conversion to 'uint8' from 'double' may alter its value [-Wfloat-conversion]
   case 0: p->r = c; p->g = X; return;
                            ^
/home/admin/main_daemon/miniz/examples/example6.c:46:18: warning: conversion to 'uint8' from 'double' may alter its value [-Wfloat-conversion]
   case 1: p->r = X; p->g = c; return;
                  ^
/home/admin/main_daemon/miniz/examples/example6.c:46:28: warning: conversion to 'uint8' from 'double' may alter its value [-Wfloat-conversion]
   case 1: p->r = X; p->g = c; return;
                            ^
/home/admin/main_daemon/miniz/examples/example6.c:47:18: warning: conversion to 'uint8' from 'double' may alter its value [-Wfloat-conversion]
   case 2: p->g = c; p->b = X; return;
                  ^
/home/admin/main_daemon/miniz/examples/example6.c:47:28: warning: conversion to 'uint8' from 'double' may alter its value [-Wfloat-conversion]
   case 2: p->g = c; p->b = X; return;
                            ^
/home/admin/main_daemon/miniz/examples/example6.c:48:18: warning: conversion to 'uint8' from 'double' may alter its value [-Wfloat-conversion]
   case 3: p->g = X; p->b = c; return;
                  ^
/home/admin/main_daemon/miniz/examples/example6.c:48:28: warning: conversion to 'uint8' from 'double' may alter its value [-Wfloat-conversion]
   case 3: p->g = X; p->b = c; return;
                            ^
/home/admin/main_daemon/miniz/examples/example6.c:49:18: warning: conversion to 'uint8' from 'double' may alter its value [-Wfloat-conversion]
   case 4: p->r = X; p->b = c; return;
                  ^
/home/admin/main_daemon/miniz/examples/example6.c:49:28: warning: conversion to 'uint8' from 'double' may alter its value [-Wfloat-conversion]
   case 4: p->r = X; p->b = c; return;
                            ^
/home/admin/main_daemon/miniz/examples/example6.c:50:18: warning: conversion to 'uint8' from 'double' may alter its value [-Wfloat-conversion]
   default:p->r = c; p->b = X;
                  ^
/home/admin/main_daemon/miniz/examples/example6.c:50:28: warning: conversion to 'uint8' from 'double' may alter its value [-Wfloat-conversion]
   default:p->r = c; p->b = X;
                            ^
/home/admin/main_daemon/miniz/examples/example6.c: In function 'main':
/home/admin/main_daemon/miniz/examples/example6.c:85:35: warning: conversion to 'size_t' from 'int' may change the sign of the result [-Wsign-conversion]
   uint8 *pImage = (uint8 *)malloc(iXmax * 3 * iYmax);
                                   ^
/home/admin/main_daemon/miniz/examples/example6.c:119:18: warning: conversion to 'uint8' from 'int' may alter its value [-Wconversion]
       color[1] = (uint8)Iteration >> 8;
                  ^
/home/admin/main_daemon/miniz/examples/example6.c:135:25: warning: conversion to 'uint' from 'int' may change the sign of the result [-Wsign-conversion]
       uint Iterations = color[0] | (color[1] << 8U);
                         ^
/home/admin/main_daemon/miniz/examples/example6.c:137:18: warning: conversion to 'int' from 'uint' may change the sign of the result [-Wsign-conversion]
       hsv_to_rgb(Iterations, MinIter, MaxIter, (rgb_t *)color);
                  ^
Linking C executable ../../bin/example6.exe
[100%] Built target example6
-- The C compiler identification is GNU
-- Check for working C compiler: /usr/bin/x86_64-w64-mingw32-gcc
-- Check for working C compiler: /usr/bin/x86_64-w64-mingw32-gcc -- works
-- Detecting C compiler ABI info
-- Detecting C compiler ABI info - done
-- Configuring done
-- Generating done
-- Build files have been written to: /home/admin/main_daemon/miniz/build/mingw64
Scanning dependencies of target miniz
[ 10%] Building C object CMakeFiles/miniz.dir/miniz.c.obj
[ 20%] Building C object CMakeFiles/miniz.dir/miniz_zip.c.obj
In file included from /home/admin/main_daemon/miniz/miniz.h:115:0,
                 from /home/admin/main_daemon/miniz/miniz_zip.h:3,
                 from /home/admin/main_daemon/miniz/miniz_zip.c:29:
/home/admin/main_daemon/miniz/miniz_zip.c: In function 'mz_zip_reader_locate_header_sig':
/home/admin/main_daemon/miniz/miniz_zip.c:616:44: warning: conversion to 'long long unsigned int' from 'mz_int64' may change the sign of the result [-Wsign-conversion]
         cur_file_ofs = MZ_MAX(cur_file_ofs - (sizeof(buf_u32) - 3), 0);
                                            ^
/home/admin/main_daemon/miniz/miniz_common.h:56:25: note: in definition of macro 'MZ_MAX'
 #define MZ_MAX(a, b) (((a) > (b)) ? (a) : (b))
                         ^
/home/admin/main_daemon/miniz/miniz_zip.c:616:44: warning: conversion to 'long long unsigned int' from 'mz_int64' may change the sign of the result [-Wsign-conversion]
         cur_file_ofs = MZ_MAX(cur_file_ofs - (sizeof(buf_u32) - 3), 0);
                                            ^
/home/admin/main_daemon/miniz/miniz_common.h:56:38: note: in definition of macro 'MZ_MAX'
 #define MZ_MAX(a, b) (((a) > (b)) ? (a) : (b))
                                      ^
/home/admin/main_daemon/miniz/miniz_common.h:56:22: warning: conversion to 'mz_int64' from 'long long unsigned int' may change the sign of the result [-Wsign-conversion]
 #define MZ_MAX(a, b) (((a) > (b)) ? (a) : (b))
                      ^
/home/admin/main_daemon/miniz/miniz_zip.c:616:24: note: in expansion of macro 'MZ_MAX'
         cur_file_ofs = MZ_MAX(cur_file_ofs - (sizeof(buf_u32) - 3), 0);
                        ^
/home/admin/main_daemon/miniz/miniz_zip.c: In function 'mz_zip_reader_read_central_dir':
/home/admin/main_daemon/miniz/miniz_zip.c:807:48: warning: conversion to 'mz_uint32' from 'long long unsigned int' may alter its value [-Wconversion]
                         extra_size_remaining = extra_size_remaining - sizeof(mz_uint16) * 2 - field_data_size;
                                                ^
/home/admin/main_daemon/miniz/miniz_zip.c: In function 'mz_zip_file_read_func':
/home/admin/main_daemon/miniz/miniz_zip.c:978:47: warning: conversion to 'long long int' from 'mz_uint64' may change the sign of the result [-Wsign-conversion]
          (MZ_FSEEK64(pZip->m_pState->m_pFile, file_ofs, SEEK_SET)))
                                               ^
/home/admin/main_daemon/miniz/miniz_zip.c: In function 'mz_zip_file_stat_internal':
/home/admin/main_daemon/miniz/miniz_zip.c:1262:49: warning: conversion to 'mz_uint32' from 'long long unsigned int' may alter its value [-Wconversion]
                         field_data_remaining -= sizeof(mz_uint64);
                                                 ^
/home/admin/main_daemon/miniz/miniz_zip.c:1272:49: warning: conversion to 'mz_uint32' from 'long long unsigned int' may alter its value [-Wconversion]
                         field_data_remaining -= sizeof(mz_uint64);
                                                 ^
/home/admin/main_daemon/miniz/miniz_zip.c:1282:49: warning: conversion to 'mz_uint32' from 'long long unsigned int' may alter its value [-Wconversion]
                         field_data_remaining -= sizeof(mz_uint64);
                                                 ^
/home/admin/main_daemon/miniz/miniz_zip.c:1289:40: warning: conversion to 'mz_uint32' from 'long long unsigned int' may alter its value [-Wconversion]
                 extra_size_remaining = extra_size_remaining - sizeof(mz_uint16) * 2 - field_data_size;
                                        ^
/home/admin/main_daemon/miniz/miniz_zip.c: In function 'mz_zip_validate_file':
/home/admin/main_daemon/miniz/miniz_zip.c:2349:32: warning: conversion to 'mz_uint32' from 'long long unsigned int' may alter its value [-Wconversion]
             field_total_size = field_data_size + sizeof(mz_uint16) * 2;
                                ^
/home/admin/main_daemon/miniz/miniz_zip.c: In function 'mz_zip_file_write_func':
/home/admin/main_daemon/miniz/miniz_zip.c:2789:112: warning: conversion to 'long long int' from 'mz_uint64' may change the sign of the result [-Wsign-conversion]
     if (((mz_int64)file_ofs < 0) || (((cur_ofs != (mz_int64)file_ofs)) && (MZ_FSEEK64(pZip->m_pState->m_pFile, file_ofs, SEEK_SET))))
                                                                                                                ^
/home/admin/main_daemon/miniz/miniz_zip.c: In function 'mz_zip_writer_create_zip64_extra_data':
/home/admin/main_daemon/miniz/miniz_zip.c:3002:23: warning: conversion to 'mz_uint32' from 'long long unsigned int' may alter its value [-Wconversion]
         field_size += sizeof(mz_uint64);
                       ^
/home/admin/main_daemon/miniz/miniz_zip.c:3009:23: warning: conversion to 'mz_uint32' from 'long long unsigned int' may alter its value [-Wconversion]
         field_size += sizeof(mz_uint64);
                       ^
/home/admin/main_daemon/miniz/miniz_zip.c:3016:23: warning: conversion to 'mz_uint32' from 'long long unsigned int' may alter its value [-Wconversion]
         field_size += sizeof(mz_uint64);
                       ^
/home/admin/main_daemon/miniz/miniz_zip.c: In function 'mz_zip_writer_update_zip64_extension_block':
/home/admin/main_daemon/miniz/miniz_zip.c:3840:32: warning: conversion to 'mz_uint32' from 'long long unsigned int' may alter its value [-Wconversion]
             field_total_size = field_data_size + sizeof(mz_uint16) * 2;
                                ^
/home/admin/main_daemon/miniz/miniz_zip.c: In function 'mz_zip_writer_add_from_zip_reader':
/home/admin/main_daemon/miniz/miniz_zip.c:3973:32: warning: conversion to 'mz_uint32' from 'long long unsigned int' may alter its value [-Wconversion]
             field_total_size = field_data_size + sizeof(mz_uint16) * 2;
                                ^
[ 30%] Building C object CMakeFiles/miniz.dir/miniz_tinfl.c.obj
/home/admin/main_daemon/miniz/miniz_tinfl.c: In function 'tinfl_decompress':
/home/admin/main_daemon/miniz/miniz_tinfl.c:101:33: warning: conversion to 'tinfl_bit_buf_t' from 'int' may change the sign of the result [-Wsign-conversion]
         b = (mz_uint32)(bit_buf & ((1 << (n)) - 1));      \
                                 ^
/home/admin/main_daemon/miniz/miniz_tinfl.c:297:21: note: in expansion of macro 'TINFL_GET_BITS'
                     TINFL_GET_BITS(11, r->m_table_sizes[counter], (mz_uint8)("\05\05\04"[counter]));
                     ^
/home/admin/main_daemon/miniz/miniz_tinfl.c:117:24: warning: conversion to 'mz_uint' from 'int' may change the sign of the result [-Wsign-conversion]
             code_len = temp >> 9;                                              \
                        ^
/home/admin/main_daemon/miniz/miniz_tinfl.c:151:17: note: in expansion of macro 'TINFL_HUFF_BITBUF_FILL'
                 TINFL_HUFF_BITBUF_FILL(state_index, pHuff);                                                                         \
                 ^
/home/admin/main_daemon/miniz/miniz_tinfl.c:376:25: note: in expansion of macro 'TINFL_HUFF_DECODE'
                         TINFL_HUFF_DECODE(16, dist, &r->m_tables[2]);
                         ^
/home/admin/main_daemon/miniz/miniz_tinfl.c:126:46: warning: conversion to 'tinfl_bit_buf_t' from 'int' may change the sign of the result [-Wsign-conversion]
                 temp = (pHuff)->m_tree[~temp + ((bit_buf >> code_len++) & 1)]; \
                                              ^
/home/admin/main_daemon/miniz/miniz_tinfl.c:151:17: note: in expansion of macro 'TINFL_HUFF_BITBUF_FILL'
                 TINFL_HUFF_BITBUF_FILL(state_index, pHuff);                                                                         \
                 ^
/home/admin/main_daemon/miniz/miniz_tinfl.c:376:25: note: in expansion of macro 'TINFL_HUFF_DECODE'
                         TINFL_HUFF_DECODE(16, dist, &r->m_tables[2]);
                         ^
/home/admin/main_daemon/miniz/miniz_tinfl.c:161:24: warning: conversion to 'mz_uint' from 'int' may change the sign of the result [-Wsign-conversion]
             code_len = temp >> 9, temp &= 511;                                                                                      \
                        ^
/home/admin/main_daemon/miniz/miniz_tinfl.c:376:25: note: in expansion of macro 'TINFL_HUFF_DECODE'
                         TINFL_HUFF_DECODE(16, dist, &r->m_tables[2]);
                         ^
/home/admin/main_daemon/miniz/miniz_tinfl.c:167:46: warning: conversion to 'tinfl_bit_buf_t' from 'int' may change the sign of the result [-Wsign-conversion]
                 temp = (pHuff)->m_tree[~temp + ((bit_buf >> code_len++) & 1)];                                                      \
                                              ^
/home/admin/main_daemon/miniz/miniz_tinfl.c:376:25: note: in expansion of macro 'TINFL_HUFF_DECODE'
                         TINFL_HUFF_DECODE(16, dist, &r->m_tables[2]);
                         ^
/home/admin/main_daemon/miniz/miniz_tinfl.c:101:33: warning: conversion to 'tinfl_bit_buf_t' from 'int' may change the sign of the result [-Wsign-conversion]
         b = (mz_uint32)(bit_buf & ((1 << (n)) - 1));      \
                                 ^
/home/admin/main_daemon/miniz/miniz_tinfl.c:387:25: note: in expansion of macro 'TINFL_GET_BITS'
                         TINFL_GET_BITS(18, s, num_extra);
                         ^
/home/admin/main_daemon/miniz/miniz_tinfl.c:117:24: warning: conversion to 'mz_uint' from 'int' may change the sign of the result [-Wsign-conversion]
             code_len = temp >> 9;                                              \
                        ^
/home/admin/main_daemon/miniz/miniz_tinfl.c:151:17: note: in expansion of macro 'TINFL_HUFF_BITBUF_FILL'
                 TINFL_HUFF_BITBUF_FILL(state_index, pHuff);                                                                         \
                 ^
/home/admin/main_daemon/miniz/miniz_tinfl.c:407:25: note: in expansion of macro 'TINFL_HUFF_DECODE'
                         TINFL_HUFF_DECODE(23, counter, &r->m_tables[0]);
                         ^
/home/admin/main_daemon/miniz/miniz_tinfl.c:126:46: warning: conversion to 'tinfl_bit_buf_t' from 'int' may change the sign of the result [-Wsign-conversion]
                 temp = (pHuff)->m_tree[~temp + ((bit_buf >> code_len++) & 1)]; \
                                              ^
/home/admin/main_daemon/miniz/miniz_tinfl.c:151:17: note: in expansion of macro 'TINFL_HUFF_BITBUF_FILL'
                 TINFL_HUFF_BITBUF_FILL(state_index, pHuff);                                                                         \
                 ^
/home/admin/main_daemon/miniz/miniz_tinfl.c:407:25: note: in expansion of macro 'TINFL_HUFF_DECODE'
                         TINFL_HUFF_DECODE(23, counter, &r->m_tables[0]);
                         ^
/home/admin/main_daemon/miniz/miniz_tinfl.c:161:24: warning: conversion to 'mz_uint' from 'int' may change the sign of the result [-Wsign-conversion]
             code_len = temp >> 9, temp &= 511;                                                                                      \
                        ^
/home/admin/main_daemon/miniz/miniz_tinfl.c:407:25: note: in expansion of macro 'TINFL_HUFF_DECODE'
                         TINFL_HUFF_DECODE(23, counter, &r->m_tables[0]);
                         ^
/home/admin/main_daemon/miniz/miniz_tinfl.c:167:46: warning: conversion to 'tinfl_bit_buf_t' from 'int' may change the sign of the result [-Wsign-conversion]
                 temp = (pHuff)->m_tree[~temp + ((bit_buf >> code_len++) & 1)];                                                      \
                                              ^
/home/admin/main_daemon/miniz/miniz_tinfl.c:407:25: note: in expansion of macro 'TINFL_HUFF_DECODE'
                         TINFL_HUFF_DECODE(23, counter, &r->m_tables[0]);
                         ^
/home/admin/main_daemon/miniz/miniz_tinfl.c:442:68: warning: conversion to 'tinfl_bit_buf_t' from 'int' may change the sign of the result [-Wsign-conversion]
                                 sym2 = r->m_tables[0].m_tree[~sym2 + ((bit_buf >> code_len++) & 1)];
                                                                    ^
/home/admin/main_daemon/miniz/miniz_tinfl.c:466:68: warning: conversion to 'tinfl_bit_buf_t' from 'int' may change the sign of the result [-Wsign-conversion]
                                 sym2 = r->m_tables[0].m_tree[~sym2 + ((bit_buf >> code_len++) & 1U)];
                                                                    ^
/home/admin/main_daemon/miniz/miniz_tinfl.c:101:33: warning: conversion to 'tinfl_bit_buf_t' from 'int' may change the sign of the result [-Wsign-conversion]
         b = (mz_uint32)(bit_buf & ((1 << (n)) - 1));      \
                                 ^
/home/admin/main_daemon/miniz/miniz_tinfl.c:491:21: note: in expansion of macro 'TINFL_GET_BITS'
                     TINFL_GET_BITS(25, extra_bits, num_extra);
                     ^
/home/admin/main_daemon/miniz/miniz_tinfl.c:117:24: warning: conversion to 'mz_uint' from 'int' may change the sign of the result [-Wsign-conversion]
             code_len = temp >> 9;                                              \
                        ^
/home/admin/main_daemon/miniz/miniz_tinfl.c:151:17: note: in expansion of macro 'TINFL_HUFF_BITBUF_FILL'
                 TINFL_HUFF_BITBUF_FILL(state_index, pHuff);                                                                         \
                 ^
/home/admin/main_daemon/miniz/miniz_tinfl.c:495:17: note: in expansion of macro 'TINFL_HUFF_DECODE'
                 TINFL_HUFF_DECODE(26, dist, &r->m_tables[1]);
                 ^
/home/admin/main_daemon/miniz/miniz_tinfl.c:126:46: warning: conversion to 'tinfl_bit_buf_t' from 'int' may change the sign of the result [-Wsign-conversion]
                 temp = (pHuff)->m_tree[~temp + ((bit_buf >> code_len++) & 1)]; \
                                              ^
/home/admin/main_daemon/miniz/miniz_tinfl.c:151:17: note: in expansion of macro 'TINFL_HUFF_BITBUF_FILL'
                 TINFL_HUFF_BITBUF_FILL(state_index, pHuff);                                                                         \
                 ^
/home/admin/main_daemon/miniz/miniz_tinfl.c:495:17: note: in expansion of macro 'TINFL_HUFF_DECODE'
                 TINFL_HUFF_DECODE(26, dist, &r->m_tables[1]);
                 ^
/home/admin/main_daemon/miniz/miniz_tinfl.c:161:24: warning: conversion to 'mz_uint' from 'int' may change the sign of the result [-Wsign-conversion]
             code_len = temp >> 9, temp &= 511;                                                                                      \
                        ^
/home/admin/main_daemon/miniz/miniz_tinfl.c:495:17: note: in expansion of macro 'TINFL_HUFF_DECODE'
                 TINFL_HUFF_DECODE(26, dist, &r->m_tables[1]);
                 ^
/home/admin/main_daemon/miniz/miniz_tinfl.c:167:46: warning: conversion to 'tinfl_bit_buf_t' from 'int' may change the sign of the result [-Wsign-conversion]
                 temp = (pHuff)->m_tree[~temp + ((bit_buf >> code_len++) & 1)];                                                      \
                                              ^
/home/admin/main_daemon/miniz/miniz_tinfl.c:495:17: note: in expansion of macro 'TINFL_HUFF_DECODE'
                 TINFL_HUFF_DECODE(26, dist, &r->m_tables[1]);
                 ^
/home/admin/main_daemon/miniz/miniz_tinfl.c:101:33: warning: conversion to 'tinfl_bit_buf_t' from 'int' may change the sign of the result [-Wsign-conversion]
         b = (mz_uint32)(bit_buf & ((1 << (n)) - 1));      \
                                 ^
/home/admin/main_daemon/miniz/miniz_tinfl.c:501:21: note: in expansion of macro 'TINFL_GET_BITS'
                     TINFL_GET_BITS(27, extra_bits, num_extra);
                     ^
[ 40%] Building C object CMakeFiles/miniz.dir/miniz_tdef.c.obj
/home/admin/main_daemon/miniz/miniz_tdef.c: In function 'tdefl_compress_lz_codes':
/home/admin/main_daemon/miniz/miniz_tdef.c:476:21: warning: conversion to 'mz_uint' from 'int' may change the sign of the result [-Wsign-conversion]
             flags = *pLZ_codes++ | 0x100;
                     ^
/home/admin/main_daemon/miniz/miniz_tdef.c:528:33: warning: negative integer implicitly converted to unsigned type [-Wsign-conversion]
         bit_buffer >>= (bits_in & ~7);
                                 ^
/home/admin/main_daemon/miniz/miniz_tdef.c: In function 'tdefl_create_comp_flags_from_zip_params':
/home/admin/main_daemon/miniz/miniz_tdef.c:1462:20: warning: negative integer implicitly converted to unsigned type [-Wsign-conversion]
         comp_flags &= ~TDEFL_MAX_PROBES_MASK;
                    ^
Linking C shared library ../../bin/libminiz.dll
[ 40%] Built target miniz
Scanning dependencies of target example1
[ 50%] Building C object CMakeFiles/example1.dir/examples/example1.c.obj
/home/admin/main_daemon/miniz/examples/example1.c: In function 'main':
/home/admin/main_daemon/miniz/examples/example1.c:57:16: warning: conversion to 'uint' from 'int' may change the sign of the result [-Wsign-conversion]
       uint n = 1 + (rand() % 3);
                ^
/home/admin/main_daemon/miniz/examples/example1.c:60:25: warning: conversion to 'long unsigned int' from 'int' may change the sign of the result [-Wsign-conversion]
         uint i = rand() % cmp_len;
                         ^
/home/admin/main_daemon/miniz/examples/example1.c:61:20: warning: conversion to 'uint8' from 'int' may alter its value [-Wconversion]
         pCmp[i] ^= (rand() & 0xFF);
                    ^
Linking C executable ../../bin/example1.exe
[ 50%] Built target example1
Scanning dependencies of target example2
[ 60%] Building C object CMakeFiles/example2.dir/examples/example2.c.obj
/home/admin/main_daemon/miniz/examples/example2.c: In function 'main':
/home/admin/main_daemon/miniz/examples/example2.c:94:48: warning: conversion to 'mz_uint' from 'int' may change the sign of the result [-Wsign-conversion]
     if (!mz_zip_reader_file_stat(&zip_archive, i, &file_stat))
                                                ^
/home/admin/main_daemon/miniz/examples/example2.c:101:258: warning: conversion to 'mz_uint' from 'int' may change the sign of the result [-Wsign-conversion]
     printf("Filename: \"%s\", Comment: \"%s\", Uncompressed size: %u, Compressed size: %u, Is Dir: %u\n", file_stat.m_filename, file_stat.m_comment, (uint)file_stat.m_uncomp_size, (uint)file_stat.m_comp_size, mz_zip_reader_is_file_a_directory(&zip_archive, i));
                                                                                                                                                                                                                                                                  ^
/home/admin/main_daemon/miniz/examples/example2.c:105:60: warning: conversion to 'mz_uint' from 'int' may change the sign of the result [-Wsign-conversion]
       if (!mz_zip_reader_is_file_a_directory(&zip_archive, i))
                                                            ^
Linking C executable ../../bin/example2.exe
[ 60%] Built target example2
Scanning dependencies of target example3
[ 70%] Building C object CMakeFiles/example3.dir/examples/example3.c.obj
Linking C executable ../../bin/example3.exe
[ 70%] Built target example3
Scanning dependencies of target example4
[ 80%] Building C object CMakeFiles/example4.dir/examples/example4.c.obj
/home/admin/main_daemon/miniz/examples/example4.c: In function 'tinfl_put_buf_func':
/home/admin/main_daemon/miniz/examples/example4.c:16:38: warning: conversion to 'size_t' from 'int' may change the sign of the result [-Wsign-conversion]
   return len == (int)fwrite(pBuf, 1, len, (FILE*)pUser);
                                      ^
/home/admin/main_daemon/miniz/examples/example4.c: In function 'main':
/home/admin/main_daemon/miniz/examples/example4.c:89:18: warning: conversion to 'uint' from 'long int' may change the sign of the result [-Wsign-conversion]
   outfile_size = ftell(pOutfile);
                  ^
Linking C executable ../../bin/example4.exe
[ 80%] Built target example4
Scanning dependencies of target example5
[ 90%] Building C object CMakeFiles/example5.dir/examples/example5.c.obj
/home/admin/main_daemon/miniz/examples/example5.c: In function 'main':
/home/admin/main_daemon/miniz/examples/example5.c:168:52: warning: conversion to 'int' from 'mz_uint' may change the sign of the result [-Wsign-conversion]
       status = tdefl_init(&g_deflator, NULL, NULL, comp_flags);
                                                    ^
Linking C executable ../../bin/example5.exe
[ 90%] Built target example5
Scanning dependencies of target example6
[100%] Building C object CMakeFiles/example6.dir/examples/example6.c.obj
/home/admin/main_daemon/miniz/examples/example6.c: In function 'hsv_to_rgb':
/home/admin/main_daemon/miniz/examples/example6.c:35:26: warning: conversion to 'uint8' from 'int' may alter its value [-Wconversion]
     p->r = p->g = p->b = 255 * (max - hue) / (max - min);
                          ^
/home/admin/main_daemon/miniz/examples/example6.c:39:21: warning: conversion to 'float' from 'int' may alter its value [-Wconversion]
   double c = 255.0f * saturation;
                     ^
/home/admin/main_daemon/miniz/examples/example6.c:45:18: warning: conversion to 'uint8' from 'double' may alter its value [-Wfloat-conversion]
   case 0: p->r = c; p->g = X; return;
                  ^
/home/admin/main_daemon/miniz/examples/example6.c:45:28: warning: conversion to 'uint8' from 'double' may alter its value [-Wfloat-conversion]
   case 0: p->r = c; p->g = X; return;
                            ^
/home/admin/main_daemon/miniz/examples/example6.c:46:18: warning: conversion to 'uint8' from 'double' may alter its value [-Wfloat-conversion]
   case 1: p->r = X; p->g = c; return;
                  ^
/home/admin/main_daemon/miniz/examples/example6.c:46:28: warning: conversion to 'uint8' from 'double' may alter its value [-Wfloat-conversion]
   case 1: p->r = X; p->g = c; return;
                            ^
/home/admin/main_daemon/miniz/examples/example6.c:47:18: warning: conversion to 'uint8' from 'double' may alter its value [-Wfloat-conversion]
   case 2: p->g = c; p->b = X; return;
                  ^
/home/admin/main_daemon/miniz/examples/example6.c:47:28: warning: conversion to 'uint8' from 'double' may alter its value [-Wfloat-conversion]
   case 2: p->g = c; p->b = X; return;
                            ^
/home/admin/main_daemon/miniz/examples/example6.c:48:18: warning: conversion to 'uint8' from 'double' may alter its value [-Wfloat-conversion]
   case 3: p->g = X; p->b = c; return;
                  ^
/home/admin/main_daemon/miniz/examples/example6.c:48:28: warning: conversion to 'uint8' from 'double' may alter its value [-Wfloat-conversion]
   case 3: p->g = X; p->b = c; return;
                            ^
/home/admin/main_daemon/miniz/examples/example6.c:49:18: warning: conversion to 'uint8' from 'double' may alter its value [-Wfloat-conversion]
   case 4: p->r = X; p->b = c; return;
                  ^
/home/admin/main_daemon/miniz/examples/example6.c:49:28: warning: conversion to 'uint8' from 'double' may alter its value [-Wfloat-conversion]
   case 4: p->r = X; p->b = c; return;
                            ^
/home/admin/main_daemon/miniz/examples/example6.c:50:18: warning: conversion to 'uint8' from 'double' may alter its value [-Wfloat-conversion]
   default:p->r = c; p->b = X;
                  ^
/home/admin/main_daemon/miniz/examples/example6.c:50:28: warning: conversion to 'uint8' from 'double' may alter its value [-Wfloat-conversion]
   default:p->r = c; p->b = X;
                            ^
/home/admin/main_daemon/miniz/examples/example6.c: In function 'main':
/home/admin/main_daemon/miniz/examples/example6.c:85:35: warning: conversion to 'size_t' from 'int' may change the sign of the result [-Wsign-conversion]
   uint8 *pImage = (uint8 *)malloc(iXmax * 3 * iYmax);
                                   ^
/home/admin/main_daemon/miniz/examples/example6.c:119:18: warning: conversion to 'uint8' from 'int' may alter its value [-Wconversion]
       color[1] = (uint8)Iteration >> 8;
                  ^
/home/admin/main_daemon/miniz/examples/example6.c:135:25: warning: conversion to 'uint' from 'int' may change the sign of the result [-Wsign-conversion]
       uint Iterations = color[0] | (color[1] << 8U);
                         ^
/home/admin/main_daemon/miniz/examples/example6.c:137:18: warning: conversion to 'int' from 'uint' may change the sign of the result [-Wsign-conversion]
       hsv_to_rgb(Iterations, MinIter, MaxIter, (rgb_t *)color);
                  ^
Linking C executable ../../bin/example6.exe
[100%] Built target example6
````
