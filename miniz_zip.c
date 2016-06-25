
#include "miniz_zip.h"

// ------------------- .ZIP archive reading

#ifdef MINIZ_NO_STDIO
#define MZ_FILE void *
#else
#include <stdio.h>
#include <sys/stat.h>

#if defined(_MSC_VER) || defined(__MINGW64__)
static FILE *mz_fopen(const char *pFilename, const char *pMode)
{
    FILE *pFile = NULL;
    fopen_s(&pFile, pFilename, pMode);
    return pFile;
}
static FILE *mz_freopen(const char *pPath, const char *pMode, FILE *pStream)
{
    FILE *pFile = NULL;
    if (freopen_s(&pFile, pPath, pMode, pStream))
        return NULL;
    return pFile;
}
#ifndef MINIZ_NO_TIME
#include <sys/utime.h>
#endif
#define MZ_FILE FILE
#define MZ_FOPEN mz_fopen
#define MZ_FCLOSE fclose
#define MZ_FREAD fread
#define MZ_FWRITE fwrite
#define MZ_FTELL64 _ftelli64
#define MZ_FSEEK64 _fseeki64
#define MZ_FILE_STAT_STRUCT _stat
#define MZ_FILE_STAT _stat
#define MZ_FFLUSH fflush
#define MZ_FREOPEN mz_freopen
#define MZ_DELETE_FILE remove
#elif defined(__MINGW32__)
#ifndef MINIZ_NO_TIME
#include <sys/utime.h>
#endif
#define MZ_FILE FILE
#define MZ_FOPEN(f, m) fopen(f, m)
#define MZ_FCLOSE fclose
#define MZ_FREAD fread
#define MZ_FWRITE fwrite
#define MZ_FTELL64 ftello64
#define MZ_FSEEK64 fseeko64
#define MZ_FILE_STAT_STRUCT _stat
#define MZ_FILE_STAT _stat
#define MZ_FFLUSH fflush
#define MZ_FREOPEN(f, m, s) freopen(f, m, s)
#define MZ_DELETE_FILE remove
#elif defined(__TINYC__)
#ifndef MINIZ_NO_TIME
#include <sys/utime.h>
#endif
#define MZ_FILE FILE
#define MZ_FOPEN(f, m) fopen(f, m)
#define MZ_FCLOSE fclose
#define MZ_FREAD fread
#define MZ_FWRITE fwrite
#define MZ_FTELL64 ftell
#define MZ_FSEEK64 fseek
#define MZ_FILE_STAT_STRUCT stat
#define MZ_FILE_STAT stat
#define MZ_FFLUSH fflush
#define MZ_FREOPEN(f, m, s) freopen(f, m, s)
#define MZ_DELETE_FILE remove
#elif defined(__GNUC__) && _LARGEFILE64_SOURCE
#ifndef MINIZ_NO_TIME
#include <utime.h>
#endif
#define MZ_FILE FILE
#define MZ_FOPEN(f, m) fopen64(f, m)
#define MZ_FCLOSE fclose
#define MZ_FREAD fread
#define MZ_FWRITE fwrite
#define MZ_FTELL64 ftello64
#define MZ_FSEEK64 fseeko64
#define MZ_FILE_STAT_STRUCT stat64
#define MZ_FILE_STAT stat64
#define MZ_FFLUSH fflush
#define MZ_FREOPEN(p, m, s) freopen64(p, m, s)
#define MZ_DELETE_FILE remove
#else
#ifndef MINIZ_NO_TIME
#include <utime.h>
#endif
#define MZ_FILE FILE
#define MZ_FOPEN(f, m) fopen(f, m)
#define MZ_FCLOSE fclose
#define MZ_FREAD fread
#define MZ_FWRITE fwrite
#define MZ_FTELL64 ftello
#define MZ_FSEEK64 fseeko
#define MZ_FILE_STAT_STRUCT stat
#define MZ_FILE_STAT stat
#define MZ_FFLUSH fflush
#define MZ_FREOPEN(f, m, s) freopen(f, m, s)
#define MZ_DELETE_FILE remove
#endif // #ifdef _MSC_VER
#endif // #ifdef MINIZ_NO_STDIO

#define MZ_TOLOWER(c) ((((c) >= 'A') && ((c) <= 'Z')) ? ((c) - 'A' + 'a') : (c))

// Various ZIP archive enums. To completely avoid cross platform compiler alignment and platform endian issues, miniz.c doesn't use structs for any of this stuff.
enum
{
    // ZIP archive identifiers and record sizes
    MZ_ZIP_END_OF_CENTRAL_DIR_HEADER_SIG = 0x06054b50,
    MZ_ZIP_CENTRAL_DIR_HEADER_SIG = 0x02014b50,
    MZ_ZIP_LOCAL_DIR_HEADER_SIG = 0x04034b50,
    MZ_ZIP_LOCAL_DIR_HEADER_SIZE = 30,
    MZ_ZIP_CENTRAL_DIR_HEADER_SIZE = 46,
    MZ_ZIP_END_OF_CENTRAL_DIR_HEADER_SIZE = 22,
    // Central directory header record offsets
    MZ_ZIP_CDH_SIG_OFS = 0,
    MZ_ZIP_CDH_VERSION_MADE_BY_OFS = 4,
    MZ_ZIP_CDH_VERSION_NEEDED_OFS = 6,
    MZ_ZIP_CDH_BIT_FLAG_OFS = 8,
    MZ_ZIP_CDH_METHOD_OFS = 10,
    MZ_ZIP_CDH_FILE_TIME_OFS = 12,
    MZ_ZIP_CDH_FILE_DATE_OFS = 14,
    MZ_ZIP_CDH_CRC32_OFS = 16,
    MZ_ZIP_CDH_COMPRESSED_SIZE_OFS = 20,
    MZ_ZIP_CDH_DECOMPRESSED_SIZE_OFS = 24,
    MZ_ZIP_CDH_FILENAME_LEN_OFS = 28,
    MZ_ZIP_CDH_EXTRA_LEN_OFS = 30,
    MZ_ZIP_CDH_COMMENT_LEN_OFS = 32,
    MZ_ZIP_CDH_DISK_START_OFS = 34,
    MZ_ZIP_CDH_INTERNAL_ATTR_OFS = 36,
    MZ_ZIP_CDH_EXTERNAL_ATTR_OFS = 38,
    MZ_ZIP_CDH_LOCAL_HEADER_OFS = 42,
    // Local directory header offsets
    MZ_ZIP_LDH_SIG_OFS = 0,
    MZ_ZIP_LDH_VERSION_NEEDED_OFS = 4,
    MZ_ZIP_LDH_BIT_FLAG_OFS = 6,
    MZ_ZIP_LDH_METHOD_OFS = 8,
    MZ_ZIP_LDH_FILE_TIME_OFS = 10,
    MZ_ZIP_LDH_FILE_DATE_OFS = 12,
    MZ_ZIP_LDH_CRC32_OFS = 14,
    MZ_ZIP_LDH_COMPRESSED_SIZE_OFS = 18,
    MZ_ZIP_LDH_DECOMPRESSED_SIZE_OFS = 22,
    MZ_ZIP_LDH_FILENAME_LEN_OFS = 26,
    MZ_ZIP_LDH_EXTRA_LEN_OFS = 28,
    // End of central directory offsets
    MZ_ZIP_ECDH_SIG_OFS = 0,
    MZ_ZIP_ECDH_NUM_THIS_DISK_OFS = 4,
    MZ_ZIP_ECDH_NUM_DISK_CDIR_OFS = 6,
    MZ_ZIP_ECDH_CDIR_NUM_ENTRIES_ON_DISK_OFS = 8,
    MZ_ZIP_ECDH_CDIR_TOTAL_ENTRIES_OFS = 10,
    MZ_ZIP_ECDH_CDIR_SIZE_OFS = 12,
    MZ_ZIP_ECDH_CDIR_OFS_OFS = 16,
    MZ_ZIP_ECDH_COMMENT_SIZE_OFS = 20,
};

typedef struct
{
    void *m_p;
    size_t m_size, m_capacity;
    mz_uint m_element_size;
} mz_zip_array;

struct mz_zip_internal_state_tag
{
    mz_zip_array m_central_dir;
    mz_zip_array m_central_dir_offsets;
    mz_zip_array m_sorted_central_dir_offsets;
    MZ_FILE *m_pFile;
    void *m_pMem;
    size_t m_mem_size;
    size_t m_mem_capacity;
};

#define MZ_ZIP_ARRAY_SET_ELEMENT_SIZE(array_ptr, element_size) (array_ptr)->m_element_size = element_size
#define MZ_ZIP_ARRAY_ELEMENT(array_ptr, element_type, index) ((element_type *)((array_ptr)->m_p))[index]

static MZ_FORCEINLINE void mz_zip_array_clear(mz_zip_archive *pZip, mz_zip_array *pArray)
{
    pZip->m_pFree(pZip->m_pAlloc_opaque, pArray->m_p);
    memset(pArray, 0, sizeof(mz_zip_array));
}

static mz_bool mz_zip_array_ensure_capacity(mz_zip_archive *pZip, mz_zip_array *pArray, size_t min_new_capacity, mz_uint growing)
{
    void *pNew_p;
    size_t new_capacity = min_new_capacity;
    MZ_ASSERT(pArray->m_element_size);
    if (pArray->m_capacity >= min_new_capacity)
        return MZ_TRUE;
    if (growing)
    {
        new_capacity = MZ_MAX(1, pArray->m_capacity);
        while (new_capacity < min_new_capacity)
            new_capacity *= 2;
    }
    if (NULL == (pNew_p = pZip->m_pRealloc(pZip->m_pAlloc_opaque, pArray->m_p, pArray->m_element_size, new_capacity)))
        return MZ_FALSE;
    pArray->m_p = pNew_p;
    pArray->m_capacity = new_capacity;
    return MZ_TRUE;
}

static MZ_FORCEINLINE mz_bool mz_zip_array_reserve(mz_zip_archive *pZip, mz_zip_array *pArray, size_t new_capacity, mz_uint growing)
{
    if (new_capacity > pArray->m_capacity)
    {
        if (!mz_zip_array_ensure_capacity(pZip, pArray, new_capacity, growing))
            return MZ_FALSE;
    }
    return MZ_TRUE;
}

static MZ_FORCEINLINE mz_bool mz_zip_array_resize(mz_zip_archive *pZip, mz_zip_array *pArray, size_t new_size, mz_uint growing)
{
    if (new_size > pArray->m_capacity)
    {
        if (!mz_zip_array_ensure_capacity(pZip, pArray, new_size, growing))
            return MZ_FALSE;
    }
    pArray->m_size = new_size;
    return MZ_TRUE;
}

static MZ_FORCEINLINE mz_bool mz_zip_array_ensure_room(mz_zip_archive *pZip, mz_zip_array *pArray, size_t n)
{
    return mz_zip_array_reserve(pZip, pArray, pArray->m_size + n, MZ_TRUE);
}

static MZ_FORCEINLINE mz_bool mz_zip_array_push_back(mz_zip_archive *pZip, mz_zip_array *pArray, const void *pElements, size_t n)
{
    size_t orig_size = pArray->m_size;
    if (!mz_zip_array_resize(pZip, pArray, orig_size + n, MZ_TRUE))
        return MZ_FALSE;
    memcpy((mz_uint8 *)pArray->m_p + orig_size * pArray->m_element_size, pElements, n * pArray->m_element_size);
    return MZ_TRUE;
}

#ifndef MINIZ_NO_TIME
static time_t mz_zip_dos_to_time_t(int dos_time, int dos_date)
{
    struct tm tm;
    memset(&tm, 0, sizeof(tm));
    tm.tm_isdst = -1;
    tm.tm_year = ((dos_date >> 9) & 127) + 1980 - 1900;
    tm.tm_mon = ((dos_date >> 5) & 15) - 1;
    tm.tm_mday = dos_date & 31;
    tm.tm_hour = (dos_time >> 11) & 31;
    tm.tm_min = (dos_time >> 5) & 63;
    tm.tm_sec = (dos_time << 1) & 62;
    return mktime(&tm);
}

static void mz_zip_time_to_dos_time(time_t time, mz_uint16 *pDOS_time, mz_uint16 *pDOS_date)
{
#ifdef _MSC_VER
    struct tm tm_struct;
    struct tm *tm = &tm_struct;
    errno_t err = localtime_s(tm, &time);
    if (err)
    {
        *pDOS_date = 0;
        *pDOS_time = 0;
        return;
    }
#else
    struct tm *tm = localtime(&time);
#endif
    *pDOS_time = (mz_uint16)(((tm->tm_hour) << 11) + ((tm->tm_min) << 5) + ((tm->tm_sec) >> 1));
    *pDOS_date = (mz_uint16)(((tm->tm_year + 1900 - 1980) << 9) + ((tm->tm_mon + 1) << 5) + tm->tm_mday);
}
#endif

#ifndef MINIZ_NO_STDIO
static mz_bool mz_zip_get_file_modified_time(const char *pFilename, mz_uint16 *pDOS_time, mz_uint16 *pDOS_date)
{
#ifdef MINIZ_NO_TIME
    (void)pFilename;
    *pDOS_date = *pDOS_time = 0;
#else
    struct MZ_FILE_STAT_STRUCT file_stat;
    // On Linux with x86 glibc, this call will fail on large files (>= 0x80000000 bytes) unless you compiled with _LARGEFILE64_SOURCE. Argh.
    if (MZ_FILE_STAT(pFilename, &file_stat) != 0)
        return MZ_FALSE;
    mz_zip_time_to_dos_time(file_stat.st_mtime, pDOS_time, pDOS_date);
#endif // #ifdef MINIZ_NO_TIME
    return MZ_TRUE;
}

#ifndef MINIZ_NO_TIME
static mz_bool mz_zip_set_file_times(const char *pFilename, time_t access_time, time_t modified_time)
{
    struct utimbuf t;
    t.actime = access_time;
    t.modtime = modified_time;
    return !utime(pFilename, &t);
}
#endif // #ifndef MINIZ_NO_TIME
#endif // #ifndef MINIZ_NO_STDIO

static mz_bool mz_zip_reader_init_internal(mz_zip_archive *pZip, mz_uint32 flags)
{
    (void)flags;
    if ((!pZip) || (pZip->m_pState) || (pZip->m_zip_mode != MZ_ZIP_MODE_INVALID))
        return MZ_FALSE;

    if (!pZip->m_pAlloc)
        pZip->m_pAlloc = miniz_def_alloc_func;
    if (!pZip->m_pFree)
        pZip->m_pFree = miniz_def_free_func;
    if (!pZip->m_pRealloc)
        pZip->m_pRealloc = miniz_def_realloc_func;

    pZip->m_zip_mode = MZ_ZIP_MODE_READING;
    pZip->m_archive_size = 0;
    pZip->m_central_directory_file_ofs = 0;
    pZip->m_total_files = 0;

    if (NULL == (pZip->m_pState = (mz_zip_internal_state *)pZip->m_pAlloc(pZip->m_pAlloc_opaque, 1, sizeof(mz_zip_internal_state))))
        return MZ_FALSE;
    memset(pZip->m_pState, 0, sizeof(mz_zip_internal_state));
    MZ_ZIP_ARRAY_SET_ELEMENT_SIZE(&pZip->m_pState->m_central_dir, sizeof(mz_uint8));
    MZ_ZIP_ARRAY_SET_ELEMENT_SIZE(&pZip->m_pState->m_central_dir_offsets, sizeof(mz_uint32));
    MZ_ZIP_ARRAY_SET_ELEMENT_SIZE(&pZip->m_pState->m_sorted_central_dir_offsets, sizeof(mz_uint32));
    return MZ_TRUE;
}

static MZ_FORCEINLINE mz_bool mz_zip_reader_filename_less(const mz_zip_array *pCentral_dir_array, const mz_zip_array *pCentral_dir_offsets, mz_uint l_index, mz_uint r_index)
{
    const mz_uint8 *pL = &MZ_ZIP_ARRAY_ELEMENT(pCentral_dir_array, mz_uint8, MZ_ZIP_ARRAY_ELEMENT(pCentral_dir_offsets, mz_uint32, l_index)), *pE;
    const mz_uint8 *pR = &MZ_ZIP_ARRAY_ELEMENT(pCentral_dir_array, mz_uint8, MZ_ZIP_ARRAY_ELEMENT(pCentral_dir_offsets, mz_uint32, r_index));
    mz_uint l_len = MZ_READ_LE16(pL + MZ_ZIP_CDH_FILENAME_LEN_OFS), r_len = MZ_READ_LE16(pR + MZ_ZIP_CDH_FILENAME_LEN_OFS);
    mz_uint8 l = 0, r = 0;
    pL += MZ_ZIP_CENTRAL_DIR_HEADER_SIZE;
    pR += MZ_ZIP_CENTRAL_DIR_HEADER_SIZE;
    pE = pL + MZ_MIN(l_len, r_len);
    while (pL < pE)
    {
        if ((l = MZ_TOLOWER(*pL)) != (r = MZ_TOLOWER(*pR)))
            break;
        pL++;
        pR++;
    }
    return (pL == pE) ? (l_len < r_len) : (l < r);
}

#define MZ_SWAP_UINT32(a, b) \
    do                       \
    {                        \
        mz_uint32 t = a;     \
        a = b;               \
        b = t;               \
    }                        \
    MZ_MACRO_END

// Heap sort of lowercased filenames, used to help accelerate plain central directory searches by mz_zip_reader_locate_file(). (Could also use qsort(), but it could allocate memory.)
static void mz_zip_reader_sort_central_dir_offsets_by_filename(mz_zip_archive *pZip)
{
    mz_zip_internal_state *pState = pZip->m_pState;
    const mz_zip_array *pCentral_dir_offsets = &pState->m_central_dir_offsets;
    const mz_zip_array *pCentral_dir = &pState->m_central_dir;
    mz_uint32 *pIndices = &MZ_ZIP_ARRAY_ELEMENT(&pState->m_sorted_central_dir_offsets, mz_uint32, 0);
    const int size = pZip->m_total_files;
    int start = (size - 2) >> 1, end;
    while (start >= 0)
    {
        int child, root = start;
        for (;;)
        {
            if ((child = (root << 1) + 1) >= size)
                break;
            child += (((child + 1) < size) && (mz_zip_reader_filename_less(pCentral_dir, pCentral_dir_offsets, pIndices[child], pIndices[child + 1])));
            if (!mz_zip_reader_filename_less(pCentral_dir, pCentral_dir_offsets, pIndices[root], pIndices[child]))
                break;
            MZ_SWAP_UINT32(pIndices[root], pIndices[child]);
            root = child;
        }
        start--;
    }

    end = size - 1;
    while (end > 0)
    {
        int child, root = 0;
        MZ_SWAP_UINT32(pIndices[end], pIndices[0]);
        for (;;)
        {
            if ((child = (root << 1) + 1) >= end)
                break;
            child += (((child + 1) < end) && mz_zip_reader_filename_less(pCentral_dir, pCentral_dir_offsets, pIndices[child], pIndices[child + 1]));
            if (!mz_zip_reader_filename_less(pCentral_dir, pCentral_dir_offsets, pIndices[root], pIndices[child]))
                break;
            MZ_SWAP_UINT32(pIndices[root], pIndices[child]);
            root = child;
        }
        end--;
    }
}

static mz_bool mz_zip_reader_read_central_dir(mz_zip_archive *pZip, mz_uint32 flags)
{
    mz_uint cdir_size, num_this_disk, cdir_disk_index;
    mz_uint64 cdir_ofs;
    mz_int64 cur_file_ofs;
    const mz_uint8 *p;
    mz_uint32 buf_u32[4096 / sizeof(mz_uint32)];
    mz_uint8 *pBuf = (mz_uint8 *)buf_u32;
    mz_bool sort_central_dir = ((flags & MZ_ZIP_FLAG_DO_NOT_SORT_CENTRAL_DIRECTORY) == 0);
    // Basic sanity checks - reject files which are too small, and check the first 4 bytes of the file to make sure a local header is there.
    if (pZip->m_archive_size < MZ_ZIP_END_OF_CENTRAL_DIR_HEADER_SIZE)
        return MZ_FALSE;
    // Find the end of central directory record by scanning the file from the end towards the beginning.
    cur_file_ofs = MZ_MAX((mz_int64)pZip->m_archive_size - (mz_int64)sizeof(buf_u32), 0);
    for (;;)
    {
        int i, n = (int)MZ_MIN(sizeof(buf_u32), pZip->m_archive_size - cur_file_ofs);
        if (pZip->m_pRead(pZip->m_pIO_opaque, cur_file_ofs, pBuf, n) != (mz_uint)n)
            return MZ_FALSE;
        for (i = n - 4; i >= 0; --i)
            if (MZ_READ_LE32(pBuf + i) == MZ_ZIP_END_OF_CENTRAL_DIR_HEADER_SIG)
                break;
        if (i >= 0)
        {
            cur_file_ofs += i;
            break;
        }
        if ((!cur_file_ofs) || ((pZip->m_archive_size - cur_file_ofs) >= (0xFFFF + MZ_ZIP_END_OF_CENTRAL_DIR_HEADER_SIZE)))
            return MZ_FALSE;
        cur_file_ofs = MZ_MAX(cur_file_ofs - (sizeof(buf_u32) - 3), 0);
    }
    // Read and verify the end of central directory record.
    if (pZip->m_pRead(pZip->m_pIO_opaque, cur_file_ofs, pBuf, MZ_ZIP_END_OF_CENTRAL_DIR_HEADER_SIZE) != MZ_ZIP_END_OF_CENTRAL_DIR_HEADER_SIZE)
        return MZ_FALSE;
    if ((MZ_READ_LE32(pBuf + MZ_ZIP_ECDH_SIG_OFS) != MZ_ZIP_END_OF_CENTRAL_DIR_HEADER_SIG) ||
        ((pZip->m_total_files = MZ_READ_LE16(pBuf + MZ_ZIP_ECDH_CDIR_TOTAL_ENTRIES_OFS)) != MZ_READ_LE16(pBuf + MZ_ZIP_ECDH_CDIR_NUM_ENTRIES_ON_DISK_OFS)))
        return MZ_FALSE;

    num_this_disk = MZ_READ_LE16(pBuf + MZ_ZIP_ECDH_NUM_THIS_DISK_OFS);
    cdir_disk_index = MZ_READ_LE16(pBuf + MZ_ZIP_ECDH_NUM_DISK_CDIR_OFS);
    if (((num_this_disk | cdir_disk_index) != 0) && ((num_this_disk != 1) || (cdir_disk_index != 1)))
        return MZ_FALSE;

    if ((cdir_size = MZ_READ_LE32(pBuf + MZ_ZIP_ECDH_CDIR_SIZE_OFS)) < pZip->m_total_files * MZ_ZIP_CENTRAL_DIR_HEADER_SIZE)
        return MZ_FALSE;

    cdir_ofs = MZ_READ_LE32(pBuf + MZ_ZIP_ECDH_CDIR_OFS_OFS);
    if ((cdir_ofs + (mz_uint64)cdir_size) > pZip->m_archive_size)
        return MZ_FALSE;

    pZip->m_central_directory_file_ofs = cdir_ofs;

    if (pZip->m_total_files)
    {
        mz_uint i, n;

        // Read the entire central directory into a heap block, and allocate another heap block to hold the unsorted central dir file record offsets, and another to hold the sorted indices.
        if ((!mz_zip_array_resize(pZip, &pZip->m_pState->m_central_dir, cdir_size, MZ_FALSE)) ||
            (!mz_zip_array_resize(pZip, &pZip->m_pState->m_central_dir_offsets, pZip->m_total_files, MZ_FALSE)))
            return MZ_FALSE;

        if (sort_central_dir)
        {
            if (!mz_zip_array_resize(pZip, &pZip->m_pState->m_sorted_central_dir_offsets, pZip->m_total_files, MZ_FALSE))
                return MZ_FALSE;
        }

        if (pZip->m_pRead(pZip->m_pIO_opaque, cdir_ofs, pZip->m_pState->m_central_dir.m_p, cdir_size) != cdir_size)
            return MZ_FALSE;

        // Now create an index into the central directory file records, do some basic sanity checking on each record, and check for zip64 entries (which are not yet supported).
        p = (const mz_uint8 *)pZip->m_pState->m_central_dir.m_p;
        for (n = cdir_size, i = 0; i < pZip->m_total_files; ++i)
        {
            mz_uint total_header_size, comp_size, decomp_size, disk_index;
            if ((n < MZ_ZIP_CENTRAL_DIR_HEADER_SIZE) || (MZ_READ_LE32(p) != MZ_ZIP_CENTRAL_DIR_HEADER_SIG))
                return MZ_FALSE;
            MZ_ZIP_ARRAY_ELEMENT(&pZip->m_pState->m_central_dir_offsets, mz_uint32, i) = (mz_uint32)(p - (const mz_uint8 *)pZip->m_pState->m_central_dir.m_p);
            if (sort_central_dir)
                MZ_ZIP_ARRAY_ELEMENT(&pZip->m_pState->m_sorted_central_dir_offsets, mz_uint32, i) = i;
            comp_size = MZ_READ_LE32(p + MZ_ZIP_CDH_COMPRESSED_SIZE_OFS);
            decomp_size = MZ_READ_LE32(p + MZ_ZIP_CDH_DECOMPRESSED_SIZE_OFS);
            if (((!MZ_READ_LE32(p + MZ_ZIP_CDH_METHOD_OFS)) && (decomp_size != comp_size)) || (decomp_size && !comp_size) || (decomp_size == 0xFFFFFFFF) || (comp_size == 0xFFFFFFFF))
                return MZ_FALSE;
            disk_index = MZ_READ_LE16(p + MZ_ZIP_CDH_DISK_START_OFS);
            if ((disk_index != num_this_disk) && (disk_index != 1))
                return MZ_FALSE;
            if (((mz_uint64)MZ_READ_LE32(p + MZ_ZIP_CDH_LOCAL_HEADER_OFS) + MZ_ZIP_LOCAL_DIR_HEADER_SIZE + comp_size) > pZip->m_archive_size)
                return MZ_FALSE;
            if ((total_header_size = MZ_ZIP_CENTRAL_DIR_HEADER_SIZE + MZ_READ_LE16(p + MZ_ZIP_CDH_FILENAME_LEN_OFS) + MZ_READ_LE16(p + MZ_ZIP_CDH_EXTRA_LEN_OFS) + MZ_READ_LE16(p + MZ_ZIP_CDH_COMMENT_LEN_OFS)) > n)
                return MZ_FALSE;
            n -= total_header_size;
            p += total_header_size;
        }
    }

    if (sort_central_dir)
        mz_zip_reader_sort_central_dir_offsets_by_filename(pZip);

    return MZ_TRUE;
}

mz_bool mz_zip_reader_init(mz_zip_archive *pZip, mz_uint64 size, mz_uint32 flags)
{
    if ((!pZip) || (!pZip->m_pRead))
        return MZ_FALSE;
    if (!mz_zip_reader_init_internal(pZip, flags))
        return MZ_FALSE;
    pZip->m_archive_size = size;
    if (!mz_zip_reader_read_central_dir(pZip, flags))
    {
        mz_zip_reader_end(pZip);
        return MZ_FALSE;
    }
    return MZ_TRUE;
}

static size_t mz_zip_mem_read_func(void *pOpaque, mz_uint64 file_ofs, void *pBuf, size_t n)
{
    mz_zip_archive *pZip = (mz_zip_archive *)pOpaque;
    size_t s = (file_ofs >= pZip->m_archive_size) ? 0 : (size_t)MZ_MIN(pZip->m_archive_size - file_ofs, n);
    memcpy(pBuf, (const mz_uint8 *)pZip->m_pState->m_pMem + file_ofs, s);
    return s;
}

mz_bool mz_zip_reader_init_mem(mz_zip_archive *pZip, const void *pMem, size_t size, mz_uint32 flags)
{
    if (!mz_zip_reader_init_internal(pZip, flags))
        return MZ_FALSE;
    pZip->m_archive_size = size;
    pZip->m_pRead = mz_zip_mem_read_func;
    pZip->m_pIO_opaque = pZip;
#ifdef __cplusplus
    pZip->m_pState->m_pMem = const_cast<void *>(pMem);
#else
    pZip->m_pState->m_pMem = (void *)pMem;
#endif
    pZip->m_pState->m_mem_size = size;
    if (!mz_zip_reader_read_central_dir(pZip, flags))
    {
        mz_zip_reader_end(pZip);
        return MZ_FALSE;
    }
    return MZ_TRUE;
}

#ifndef MINIZ_NO_STDIO
static size_t mz_zip_file_read_func(void *pOpaque, mz_uint64 file_ofs, void *pBuf, size_t n)
{
    mz_zip_archive *pZip = (mz_zip_archive *)pOpaque;
    mz_int64 cur_ofs = MZ_FTELL64(pZip->m_pState->m_pFile);
    if (((mz_int64)file_ofs < 0) || (((cur_ofs != (mz_int64)file_ofs)) && (MZ_FSEEK64(pZip->m_pState->m_pFile, (mz_int64)file_ofs, SEEK_SET))))
        return 0;
    return MZ_FREAD(pBuf, 1, n, pZip->m_pState->m_pFile);
}

mz_bool mz_zip_reader_init_file(mz_zip_archive *pZip, const char *pFilename, mz_uint32 flags)
{
    mz_uint64 file_size;
    MZ_FILE *pFile = MZ_FOPEN(pFilename, "rb");
    if (!pFile)
        return MZ_FALSE;
    if (MZ_FSEEK64(pFile, 0, SEEK_END))
    {
        MZ_FCLOSE(pFile);
        return MZ_FALSE;
    }
    file_size = MZ_FTELL64(pFile);
    if (!mz_zip_reader_init_internal(pZip, flags))
    {
        MZ_FCLOSE(pFile);
        return MZ_FALSE;
    }
    pZip->m_pRead = mz_zip_file_read_func;
    pZip->m_pIO_opaque = pZip;
    pZip->m_pState->m_pFile = pFile;
    pZip->m_archive_size = file_size;
    if (!mz_zip_reader_read_central_dir(pZip, flags))
    {
        mz_zip_reader_end(pZip);
        return MZ_FALSE;
    }
    return MZ_TRUE;
}
#endif // #ifndef MINIZ_NO_STDIO

mz_uint mz_zip_reader_get_num_files(mz_zip_archive *pZip)
{
    return pZip ? pZip->m_total_files : 0;
}

static MZ_FORCEINLINE const mz_uint8 *mz_zip_reader_get_cdh(mz_zip_archive *pZip, mz_uint file_index)
{
    if ((!pZip) || (!pZip->m_pState) || (file_index >= pZip->m_total_files) || (pZip->m_zip_mode != MZ_ZIP_MODE_READING))
        return NULL;
    return &MZ_ZIP_ARRAY_ELEMENT(&pZip->m_pState->m_central_dir, mz_uint8, MZ_ZIP_ARRAY_ELEMENT(&pZip->m_pState->m_central_dir_offsets, mz_uint32, file_index));
}

mz_bool mz_zip_reader_is_file_encrypted(mz_zip_archive *pZip, mz_uint file_index)
{
    mz_uint m_bit_flag;
    const mz_uint8 *p = mz_zip_reader_get_cdh(pZip, file_index);
    if (!p)
        return MZ_FALSE;
    m_bit_flag = MZ_READ_LE16(p + MZ_ZIP_CDH_BIT_FLAG_OFS);
    return (m_bit_flag & 1);
}

mz_bool mz_zip_reader_is_file_a_directory(mz_zip_archive *pZip, mz_uint file_index)
{
    mz_uint filename_len, external_attr;
    const mz_uint8 *p = mz_zip_reader_get_cdh(pZip, file_index);
    if (!p)
        return MZ_FALSE;

    // First see if the filename ends with a '/' character.
    filename_len = MZ_READ_LE16(p + MZ_ZIP_CDH_FILENAME_LEN_OFS);
    if (filename_len)
    {
        if (*(p + MZ_ZIP_CENTRAL_DIR_HEADER_SIZE + filename_len - 1) == '/')
            return MZ_TRUE;
    }

    // Bugfix: This code was also checking if the internal attribute was non-zero, which wasn't correct.
    // Most/all zip writers (hopefully) set DOS file/directory attributes in the low 16-bits, so check for the DOS directory flag and ignore the source OS ID in the created by field.
    // FIXME: Remove this check? Is it necessary - we already check the filename.
    external_attr = MZ_READ_LE32(p + MZ_ZIP_CDH_EXTERNAL_ATTR_OFS);
    if ((external_attr & 0x10) != 0)
        return MZ_TRUE;

    return MZ_FALSE;
}

mz_bool mz_zip_reader_file_stat(mz_zip_archive *pZip, mz_uint file_index, mz_zip_archive_file_stat *pStat)
{
    mz_uint n;
    const mz_uint8 *p = mz_zip_reader_get_cdh(pZip, file_index);
    if ((!p) || (!pStat))
        return MZ_FALSE;

    // Unpack the central directory record.
    pStat->m_file_index = file_index;
    pStat->m_central_dir_ofs = MZ_ZIP_ARRAY_ELEMENT(&pZip->m_pState->m_central_dir_offsets, mz_uint32, file_index);
    pStat->m_version_made_by = MZ_READ_LE16(p + MZ_ZIP_CDH_VERSION_MADE_BY_OFS);
    pStat->m_version_needed = MZ_READ_LE16(p + MZ_ZIP_CDH_VERSION_NEEDED_OFS);
    pStat->m_bit_flag = MZ_READ_LE16(p + MZ_ZIP_CDH_BIT_FLAG_OFS);
    pStat->m_method = MZ_READ_LE16(p + MZ_ZIP_CDH_METHOD_OFS);
#ifndef MINIZ_NO_TIME
    pStat->m_time = mz_zip_dos_to_time_t(MZ_READ_LE16(p + MZ_ZIP_CDH_FILE_TIME_OFS), MZ_READ_LE16(p + MZ_ZIP_CDH_FILE_DATE_OFS));
#endif
    pStat->m_crc32 = MZ_READ_LE32(p + MZ_ZIP_CDH_CRC32_OFS);
    pStat->m_comp_size = MZ_READ_LE32(p + MZ_ZIP_CDH_COMPRESSED_SIZE_OFS);
    pStat->m_uncomp_size = MZ_READ_LE32(p + MZ_ZIP_CDH_DECOMPRESSED_SIZE_OFS);
    pStat->m_internal_attr = MZ_READ_LE16(p + MZ_ZIP_CDH_INTERNAL_ATTR_OFS);
    pStat->m_external_attr = MZ_READ_LE32(p + MZ_ZIP_CDH_EXTERNAL_ATTR_OFS);
    pStat->m_local_header_ofs = MZ_READ_LE32(p + MZ_ZIP_CDH_LOCAL_HEADER_OFS);

    // Copy as much of the filename and comment as possible.
    n = MZ_READ_LE16(p + MZ_ZIP_CDH_FILENAME_LEN_OFS);
    n = MZ_MIN(n, MZ_ZIP_MAX_ARCHIVE_FILENAME_SIZE - 1);
    memcpy(pStat->m_filename, p + MZ_ZIP_CENTRAL_DIR_HEADER_SIZE, n);
    pStat->m_filename[n] = '\0';

    n = MZ_READ_LE16(p + MZ_ZIP_CDH_COMMENT_LEN_OFS);
    n = MZ_MIN(n, MZ_ZIP_MAX_ARCHIVE_FILE_COMMENT_SIZE - 1);
    pStat->m_comment_size = n;
    memcpy(pStat->m_comment, p + MZ_ZIP_CENTRAL_DIR_HEADER_SIZE + MZ_READ_LE16(p + MZ_ZIP_CDH_FILENAME_LEN_OFS) + MZ_READ_LE16(p + MZ_ZIP_CDH_EXTRA_LEN_OFS), n);
    pStat->m_comment[n] = '\0';

    return MZ_TRUE;
}

mz_uint mz_zip_reader_get_filename(mz_zip_archive *pZip, mz_uint file_index, char *pFilename, mz_uint filename_buf_size)
{
    mz_uint n;
    const mz_uint8 *p = mz_zip_reader_get_cdh(pZip, file_index);
    if (!p)
    {
        if (filename_buf_size)
            pFilename[0] = '\0';
        return 0;
    }
    n = MZ_READ_LE16(p + MZ_ZIP_CDH_FILENAME_LEN_OFS);
    if (filename_buf_size)
    {
        n = MZ_MIN(n, filename_buf_size - 1);
        memcpy(pFilename, p + MZ_ZIP_CENTRAL_DIR_HEADER_SIZE, n);
        pFilename[n] = '\0';
    }
    return n + 1;
}

static MZ_FORCEINLINE mz_bool mz_zip_reader_string_equal(const char *pA, const char *pB, mz_uint len, mz_uint flags)
{
    mz_uint i;
    if (flags & MZ_ZIP_FLAG_CASE_SENSITIVE)
        return 0 == memcmp(pA, pB, len);
    for (i = 0; i < len; ++i)
        if (MZ_TOLOWER(pA[i]) != MZ_TOLOWER(pB[i]))
            return MZ_FALSE;
    return MZ_TRUE;
}

static MZ_FORCEINLINE int mz_zip_reader_filename_compare(const mz_zip_array *pCentral_dir_array, const mz_zip_array *pCentral_dir_offsets, mz_uint l_index, const char *pR, mz_uint r_len)
{
    const mz_uint8 *pL = &MZ_ZIP_ARRAY_ELEMENT(pCentral_dir_array, mz_uint8, MZ_ZIP_ARRAY_ELEMENT(pCentral_dir_offsets, mz_uint32, l_index)), *pE;
    mz_uint l_len = MZ_READ_LE16(pL + MZ_ZIP_CDH_FILENAME_LEN_OFS);
    mz_uint8 l = 0, r = 0;
    pL += MZ_ZIP_CENTRAL_DIR_HEADER_SIZE;
    pE = pL + MZ_MIN(l_len, r_len);
    while (pL < pE)
    {
        if ((l = MZ_TOLOWER(*pL)) != (r = MZ_TOLOWER(*pR)))
            break;
        pL++;
        pR++;
    }
    return (pL == pE) ? (int)(l_len - r_len) : (l - r);
}

static int mz_zip_reader_locate_file_binary_search(mz_zip_archive *pZip, const char *pFilename)
{
    mz_zip_internal_state *pState = pZip->m_pState;
    const mz_zip_array *pCentral_dir_offsets = &pState->m_central_dir_offsets;
    const mz_zip_array *pCentral_dir = &pState->m_central_dir;
    mz_uint32 *pIndices = &MZ_ZIP_ARRAY_ELEMENT(&pState->m_sorted_central_dir_offsets, mz_uint32, 0);
    const int size = pZip->m_total_files;
    const mz_uint filename_len = (mz_uint)strlen(pFilename);
    int l = 0, h = size - 1;
    while (l <= h)
    {
        int m = (l + h) >> 1, file_index = pIndices[m], comp = mz_zip_reader_filename_compare(pCentral_dir, pCentral_dir_offsets, file_index, pFilename, filename_len);
        if (!comp)
            return file_index;
        else if (comp < 0)
            l = m + 1;
        else
            h = m - 1;
    }
    return -1;
}

int mz_zip_reader_locate_file(mz_zip_archive *pZip, const char *pName, const char *pComment, mz_uint flags)
{
    mz_uint file_index;
    size_t name_len, comment_len;
    if ((!pZip) || (!pZip->m_pState) || (!pName) || (pZip->m_zip_mode != MZ_ZIP_MODE_READING))
        return -1;
    if (((flags & (MZ_ZIP_FLAG_IGNORE_PATH | MZ_ZIP_FLAG_CASE_SENSITIVE)) == 0) && (!pComment) && (pZip->m_pState->m_sorted_central_dir_offsets.m_size))
        return mz_zip_reader_locate_file_binary_search(pZip, pName);
    name_len = strlen(pName);
    if (name_len > 0xFFFF)
        return -1;
    comment_len = pComment ? strlen(pComment) : 0;
    if (comment_len > 0xFFFF)
        return -1;
    for (file_index = 0; file_index < pZip->m_total_files; file_index++)
    {
        const mz_uint8 *pHeader = &MZ_ZIP_ARRAY_ELEMENT(&pZip->m_pState->m_central_dir, mz_uint8, MZ_ZIP_ARRAY_ELEMENT(&pZip->m_pState->m_central_dir_offsets, mz_uint32, file_index));
        mz_uint filename_len = MZ_READ_LE16(pHeader + MZ_ZIP_CDH_FILENAME_LEN_OFS);
        const char *pFilename = (const char *)pHeader + MZ_ZIP_CENTRAL_DIR_HEADER_SIZE;
        if (filename_len < name_len)
            continue;
        if (comment_len)
        {
            mz_uint file_extra_len = MZ_READ_LE16(pHeader + MZ_ZIP_CDH_EXTRA_LEN_OFS), file_comment_len = MZ_READ_LE16(pHeader + MZ_ZIP_CDH_COMMENT_LEN_OFS);
            const char *pFile_comment = pFilename + filename_len + file_extra_len;
            if ((file_comment_len != comment_len) || (!mz_zip_reader_string_equal(pComment, pFile_comment, file_comment_len, flags)))
                continue;
        }
        if ((flags & MZ_ZIP_FLAG_IGNORE_PATH) && (filename_len))
        {
            int ofs = filename_len - 1;
            do
            {
                if ((pFilename[ofs] == '/') || (pFilename[ofs] == '\\') || (pFilename[ofs] == ':'))
                    break;
            } while (--ofs >= 0);
            ofs++;
            pFilename += ofs;
            filename_len -= ofs;
        }
        if ((filename_len == name_len) && (mz_zip_reader_string_equal(pName, pFilename, filename_len, flags)))
            return file_index;
    }
    return -1;
}

mz_bool mz_zip_reader_extract_to_mem_no_alloc(mz_zip_archive *pZip, mz_uint file_index, void *pBuf, size_t buf_size, mz_uint flags, void *pUser_read_buf, size_t user_read_buf_size)
{
    int status = TINFL_STATUS_DONE;
    mz_uint64 needed_size, cur_file_ofs, comp_remaining, out_buf_ofs = 0, read_buf_size, read_buf_ofs = 0, read_buf_avail;
    mz_zip_archive_file_stat file_stat;
    void *pRead_buf;
    mz_uint32 local_header_u32[(MZ_ZIP_LOCAL_DIR_HEADER_SIZE + sizeof(mz_uint32) - 1) / sizeof(mz_uint32)];
    mz_uint8 *pLocal_header = (mz_uint8 *)local_header_u32;
    tinfl_decompressor inflator;

    if ((buf_size) && (!pBuf))
        return MZ_FALSE;

    if (!mz_zip_reader_file_stat(pZip, file_index, &file_stat))
        return MZ_FALSE;

    // Empty file, or a directory (but not always a directory - I've seen odd zips with directories that have compressed data which inflates to 0 bytes)
    if (!file_stat.m_comp_size)
        return MZ_TRUE;

    // Entry is a subdirectory (I've seen old zips with dir entries which have compressed deflate data which inflates to 0 bytes, but these entries claim to uncompress to 512 bytes in the headers).
    // I'm torn how to handle this case - should it fail instead?
    if (mz_zip_reader_is_file_a_directory(pZip, file_index))
        return MZ_TRUE;

    // Encryption and patch files are not supported.
    if (file_stat.m_bit_flag & (1 | 32))
        return MZ_FALSE;

    // This function only supports stored and deflate.
    if ((!(flags & MZ_ZIP_FLAG_COMPRESSED_DATA)) && (file_stat.m_method != 0) && (file_stat.m_method != MZ_DEFLATED))
        return MZ_FALSE;

    // Ensure supplied output buffer is large enough.
    needed_size = (flags & MZ_ZIP_FLAG_COMPRESSED_DATA) ? file_stat.m_comp_size : file_stat.m_uncomp_size;
    if (buf_size < needed_size)
        return MZ_FALSE;

    // Read and parse the local directory entry.
    cur_file_ofs = file_stat.m_local_header_ofs;
    if (pZip->m_pRead(pZip->m_pIO_opaque, cur_file_ofs, pLocal_header, MZ_ZIP_LOCAL_DIR_HEADER_SIZE) != MZ_ZIP_LOCAL_DIR_HEADER_SIZE)
        return MZ_FALSE;
    if (MZ_READ_LE32(pLocal_header) != MZ_ZIP_LOCAL_DIR_HEADER_SIG)
        return MZ_FALSE;

    cur_file_ofs += MZ_ZIP_LOCAL_DIR_HEADER_SIZE + MZ_READ_LE16(pLocal_header + MZ_ZIP_LDH_FILENAME_LEN_OFS) + MZ_READ_LE16(pLocal_header + MZ_ZIP_LDH_EXTRA_LEN_OFS);
    if ((cur_file_ofs + file_stat.m_comp_size) > pZip->m_archive_size)
        return MZ_FALSE;

    if ((flags & MZ_ZIP_FLAG_COMPRESSED_DATA) || (!file_stat.m_method))
    {
        // The file is stored or the caller has requested the compressed data.
        if (pZip->m_pRead(pZip->m_pIO_opaque, cur_file_ofs, pBuf, (size_t)needed_size) != needed_size)
            return MZ_FALSE;
        return ((flags & MZ_ZIP_FLAG_COMPRESSED_DATA) != 0) || (mz_crc32(MZ_CRC32_INIT, (const mz_uint8 *)pBuf, (size_t)file_stat.m_uncomp_size) == file_stat.m_crc32);
    }

    // Decompress the file either directly from memory or from a file input buffer.
    tinfl_init(&inflator);

    if (pZip->m_pState->m_pMem)
    {
        // Read directly from the archive in memory.
        pRead_buf = (mz_uint8 *)pZip->m_pState->m_pMem + cur_file_ofs;
        read_buf_size = read_buf_avail = file_stat.m_comp_size;
        comp_remaining = 0;
    }
    else if (pUser_read_buf)
    {
        // Use a user provided read buffer.
        if (!user_read_buf_size)
            return MZ_FALSE;
        pRead_buf = (mz_uint8 *)pUser_read_buf;
        read_buf_size = user_read_buf_size;
        read_buf_avail = 0;
        comp_remaining = file_stat.m_comp_size;
    }
    else
    {
        // Temporarily allocate a read buffer.
        read_buf_size = MZ_MIN(file_stat.m_comp_size, MZ_ZIP_MAX_IO_BUF_SIZE);
#ifdef _MSC_VER
        if (((0, sizeof(size_t) == sizeof(mz_uint32))) && (read_buf_size > 0x7FFFFFFF))
#else
        if (((sizeof(size_t) == sizeof(mz_uint32))) && (read_buf_size > 0x7FFFFFFF))
#endif
            return MZ_FALSE;
        if (NULL == (pRead_buf = pZip->m_pAlloc(pZip->m_pAlloc_opaque, 1, (size_t)read_buf_size)))
            return MZ_FALSE;
        read_buf_avail = 0;
        comp_remaining = file_stat.m_comp_size;
    }

    do
    {
        size_t in_buf_size, out_buf_size = (size_t)(file_stat.m_uncomp_size - out_buf_ofs);
        if ((!read_buf_avail) && (!pZip->m_pState->m_pMem))
        {
            read_buf_avail = MZ_MIN(read_buf_size, comp_remaining);
            if (pZip->m_pRead(pZip->m_pIO_opaque, cur_file_ofs, pRead_buf, (size_t)read_buf_avail) != read_buf_avail)
            {
                status = TINFL_STATUS_FAILED;
                break;
            }
            cur_file_ofs += read_buf_avail;
            comp_remaining -= read_buf_avail;
            read_buf_ofs = 0;
        }
        in_buf_size = (size_t)read_buf_avail;
        status = tinfl_decompress(&inflator, (mz_uint8 *)pRead_buf + read_buf_ofs, &in_buf_size, (mz_uint8 *)pBuf, (mz_uint8 *)pBuf + out_buf_ofs, &out_buf_size, TINFL_FLAG_USING_NON_WRAPPING_OUTPUT_BUF | (comp_remaining ? TINFL_FLAG_HAS_MORE_INPUT : 0));
        read_buf_avail -= in_buf_size;
        read_buf_ofs += in_buf_size;
        out_buf_ofs += out_buf_size;
    } while (status == TINFL_STATUS_NEEDS_MORE_INPUT);

    if (status == TINFL_STATUS_DONE)
    {
        // Make sure the entire file was decompressed, and check its CRC.
        if ((out_buf_ofs != file_stat.m_uncomp_size) || (mz_crc32(MZ_CRC32_INIT, (const mz_uint8 *)pBuf, (size_t)file_stat.m_uncomp_size) != file_stat.m_crc32))
            status = TINFL_STATUS_FAILED;
    }

    if ((!pZip->m_pState->m_pMem) && (!pUser_read_buf))
        pZip->m_pFree(pZip->m_pAlloc_opaque, pRead_buf);

    return status == TINFL_STATUS_DONE;
}

mz_bool mz_zip_reader_extract_file_to_mem_no_alloc(mz_zip_archive *pZip, const char *pFilename, void *pBuf, size_t buf_size, mz_uint flags, void *pUser_read_buf, size_t user_read_buf_size)
{
    int file_index = mz_zip_reader_locate_file(pZip, pFilename, NULL, flags);
    if (file_index < 0)
        return MZ_FALSE;
    return mz_zip_reader_extract_to_mem_no_alloc(pZip, file_index, pBuf, buf_size, flags, pUser_read_buf, user_read_buf_size);
}

mz_bool mz_zip_reader_extract_to_mem(mz_zip_archive *pZip, mz_uint file_index, void *pBuf, size_t buf_size, mz_uint flags)
{
    return mz_zip_reader_extract_to_mem_no_alloc(pZip, file_index, pBuf, buf_size, flags, NULL, 0);
}

mz_bool mz_zip_reader_extract_file_to_mem(mz_zip_archive *pZip, const char *pFilename, void *pBuf, size_t buf_size, mz_uint flags)
{
    return mz_zip_reader_extract_file_to_mem_no_alloc(pZip, pFilename, pBuf, buf_size, flags, NULL, 0);
}

void *mz_zip_reader_extract_to_heap(mz_zip_archive *pZip, mz_uint file_index, size_t *pSize, mz_uint flags)
{
    mz_uint64 comp_size, uncomp_size, alloc_size;
    const mz_uint8 *p = mz_zip_reader_get_cdh(pZip, file_index);
    void *pBuf;

    if (pSize)
        *pSize = 0;
    if (!p)
        return NULL;

    comp_size = MZ_READ_LE32(p + MZ_ZIP_CDH_COMPRESSED_SIZE_OFS);
    uncomp_size = MZ_READ_LE32(p + MZ_ZIP_CDH_DECOMPRESSED_SIZE_OFS);

    alloc_size = (flags & MZ_ZIP_FLAG_COMPRESSED_DATA) ? comp_size : uncomp_size;
#ifdef _MSC_VER
    if (((0, sizeof(size_t) == sizeof(mz_uint32))) && (alloc_size > 0x7FFFFFFF))
#else
    if (((sizeof(size_t) == sizeof(mz_uint32))) && (alloc_size > 0x7FFFFFFF))
#endif
        return NULL;
    if (NULL == (pBuf = pZip->m_pAlloc(pZip->m_pAlloc_opaque, 1, (size_t)alloc_size)))
        return NULL;

    if (!mz_zip_reader_extract_to_mem(pZip, file_index, pBuf, (size_t)alloc_size, flags))
    {
        pZip->m_pFree(pZip->m_pAlloc_opaque, pBuf);
        return NULL;
    }

    if (pSize)
        *pSize = (size_t)alloc_size;
    return pBuf;
}

void *mz_zip_reader_extract_file_to_heap(mz_zip_archive *pZip, const char *pFilename, size_t *pSize, mz_uint flags)
{
    int file_index = mz_zip_reader_locate_file(pZip, pFilename, NULL, flags);
    if (file_index < 0)
    {
        if (pSize)
            *pSize = 0;
        return MZ_FALSE;
    }
    return mz_zip_reader_extract_to_heap(pZip, file_index, pSize, flags);
}

mz_bool mz_zip_reader_extract_to_callback(mz_zip_archive *pZip, mz_uint file_index, mz_file_write_func pCallback, void *pOpaque, mz_uint flags)
{
    int status = TINFL_STATUS_DONE;
    mz_uint file_crc32 = MZ_CRC32_INIT;
    mz_uint64 read_buf_size, read_buf_ofs = 0, read_buf_avail, comp_remaining, out_buf_ofs = 0, cur_file_ofs;
    mz_zip_archive_file_stat file_stat;
    void *pRead_buf = NULL;
    void *pWrite_buf = NULL;
    mz_uint32 local_header_u32[(MZ_ZIP_LOCAL_DIR_HEADER_SIZE + sizeof(mz_uint32) - 1) / sizeof(mz_uint32)];
    mz_uint8 *pLocal_header = (mz_uint8 *)local_header_u32;

    if (!mz_zip_reader_file_stat(pZip, file_index, &file_stat))
        return MZ_FALSE;

    // Empty file, or a directory (but not always a directory - I've seen odd zips with directories that have compressed data which inflates to 0 bytes)
    if (!file_stat.m_comp_size)
        return MZ_TRUE;

    // Entry is a subdirectory (I've seen old zips with dir entries which have compressed deflate data which inflates to 0 bytes, but these entries claim to uncompress to 512 bytes in the headers).
    // I'm torn how to handle this case - should it fail instead?
    if (mz_zip_reader_is_file_a_directory(pZip, file_index))
        return MZ_TRUE;

    // Encryption and patch files are not supported.
    if (file_stat.m_bit_flag & (1 | 32))
        return MZ_FALSE;

    // This function only supports stored and deflate.
    if ((!(flags & MZ_ZIP_FLAG_COMPRESSED_DATA)) && (file_stat.m_method != 0) && (file_stat.m_method != MZ_DEFLATED))
        return MZ_FALSE;

    // Read and parse the local directory entry.
    cur_file_ofs = file_stat.m_local_header_ofs;
    if (pZip->m_pRead(pZip->m_pIO_opaque, cur_file_ofs, pLocal_header, MZ_ZIP_LOCAL_DIR_HEADER_SIZE) != MZ_ZIP_LOCAL_DIR_HEADER_SIZE)
        return MZ_FALSE;
    if (MZ_READ_LE32(pLocal_header) != MZ_ZIP_LOCAL_DIR_HEADER_SIG)
        return MZ_FALSE;

    cur_file_ofs += MZ_ZIP_LOCAL_DIR_HEADER_SIZE + MZ_READ_LE16(pLocal_header + MZ_ZIP_LDH_FILENAME_LEN_OFS) + MZ_READ_LE16(pLocal_header + MZ_ZIP_LDH_EXTRA_LEN_OFS);
    if ((cur_file_ofs + file_stat.m_comp_size) > pZip->m_archive_size)
        return MZ_FALSE;

    // Decompress the file either directly from memory or from a file input buffer.
    if (pZip->m_pState->m_pMem)
    {
        pRead_buf = (mz_uint8 *)pZip->m_pState->m_pMem + cur_file_ofs;
        read_buf_size = read_buf_avail = file_stat.m_comp_size;
        comp_remaining = 0;
    }
    else
    {
        read_buf_size = MZ_MIN(file_stat.m_comp_size, MZ_ZIP_MAX_IO_BUF_SIZE);
        if (NULL == (pRead_buf = pZip->m_pAlloc(pZip->m_pAlloc_opaque, 1, (size_t)read_buf_size)))
            return MZ_FALSE;
        read_buf_avail = 0;
        comp_remaining = file_stat.m_comp_size;
    }

    if ((flags & MZ_ZIP_FLAG_COMPRESSED_DATA) || (!file_stat.m_method))
    {
        // The file is stored or the caller has requested the compressed data.
        if (pZip->m_pState->m_pMem)
        {
#ifdef _MSC_VER
            if (((0, sizeof(size_t) == sizeof(mz_uint32))) && (file_stat.m_comp_size > 0xFFFFFFFF))
#else
            if (((sizeof(size_t) == sizeof(mz_uint32))) && (file_stat.m_comp_size > 0xFFFFFFFF))
#endif
                return MZ_FALSE;
            if (pCallback(pOpaque, out_buf_ofs, pRead_buf, (size_t)file_stat.m_comp_size) != file_stat.m_comp_size)
                status = TINFL_STATUS_FAILED;
            else if (!(flags & MZ_ZIP_FLAG_COMPRESSED_DATA))
                file_crc32 = (mz_uint32)mz_crc32(file_crc32, (const mz_uint8 *)pRead_buf, (size_t)file_stat.m_comp_size);
            cur_file_ofs += file_stat.m_comp_size;
            out_buf_ofs += file_stat.m_comp_size;
            comp_remaining = 0;
        }
        else
        {
            while (comp_remaining)
            {
                read_buf_avail = MZ_MIN(read_buf_size, comp_remaining);
                if (pZip->m_pRead(pZip->m_pIO_opaque, cur_file_ofs, pRead_buf, (size_t)read_buf_avail) != read_buf_avail)
                {
                    status = TINFL_STATUS_FAILED;
                    break;
                }

                if (!(flags & MZ_ZIP_FLAG_COMPRESSED_DATA))
                    file_crc32 = (mz_uint32)mz_crc32(file_crc32, (const mz_uint8 *)pRead_buf, (size_t)read_buf_avail);

                if (pCallback(pOpaque, out_buf_ofs, pRead_buf, (size_t)read_buf_avail) != read_buf_avail)
                {
                    status = TINFL_STATUS_FAILED;
                    break;
                }
                cur_file_ofs += read_buf_avail;
                out_buf_ofs += read_buf_avail;
                comp_remaining -= read_buf_avail;
            }
        }
    }
    else
    {
        tinfl_decompressor inflator;
        tinfl_init(&inflator);

        if (NULL == (pWrite_buf = pZip->m_pAlloc(pZip->m_pAlloc_opaque, 1, TINFL_LZ_DICT_SIZE)))
            status = TINFL_STATUS_FAILED;
        else
        {
            do
            {
                mz_uint8 *pWrite_buf_cur = (mz_uint8 *)pWrite_buf + (out_buf_ofs & (TINFL_LZ_DICT_SIZE - 1));
                size_t in_buf_size, out_buf_size = TINFL_LZ_DICT_SIZE - (out_buf_ofs & (TINFL_LZ_DICT_SIZE - 1));
                if ((!read_buf_avail) && (!pZip->m_pState->m_pMem))
                {
                    read_buf_avail = MZ_MIN(read_buf_size, comp_remaining);
                    if (pZip->m_pRead(pZip->m_pIO_opaque, cur_file_ofs, pRead_buf, (size_t)read_buf_avail) != read_buf_avail)
                    {
                        status = TINFL_STATUS_FAILED;
                        break;
                    }
                    cur_file_ofs += read_buf_avail;
                    comp_remaining -= read_buf_avail;
                    read_buf_ofs = 0;
                }

                in_buf_size = (size_t)read_buf_avail;
                status = tinfl_decompress(&inflator, (const mz_uint8 *)pRead_buf + read_buf_ofs, &in_buf_size, (mz_uint8 *)pWrite_buf, pWrite_buf_cur, &out_buf_size, comp_remaining ? TINFL_FLAG_HAS_MORE_INPUT : 0);
                read_buf_avail -= in_buf_size;
                read_buf_ofs += in_buf_size;

                if (out_buf_size)
                {
                    if (pCallback(pOpaque, out_buf_ofs, pWrite_buf_cur, out_buf_size) != out_buf_size)
                    {
                        status = TINFL_STATUS_FAILED;
                        break;
                    }
                    file_crc32 = (mz_uint32)mz_crc32(file_crc32, pWrite_buf_cur, out_buf_size);
                    if ((out_buf_ofs += out_buf_size) > file_stat.m_uncomp_size)
                    {
                        status = TINFL_STATUS_FAILED;
                        break;
                    }
                }
            } while ((status == TINFL_STATUS_NEEDS_MORE_INPUT) || (status == TINFL_STATUS_HAS_MORE_OUTPUT));
        }
    }

    if ((status == TINFL_STATUS_DONE) && (!(flags & MZ_ZIP_FLAG_COMPRESSED_DATA)))
    {
        // Make sure the entire file was decompressed, and check its CRC.
        if ((out_buf_ofs != file_stat.m_uncomp_size) || (file_crc32 != file_stat.m_crc32))
            status = TINFL_STATUS_FAILED;
    }

    if (!pZip->m_pState->m_pMem)
        pZip->m_pFree(pZip->m_pAlloc_opaque, pRead_buf);
    if (pWrite_buf)
        pZip->m_pFree(pZip->m_pAlloc_opaque, pWrite_buf);

    return status == TINFL_STATUS_DONE;
}

mz_bool mz_zip_reader_extract_file_to_callback(mz_zip_archive *pZip, const char *pFilename, mz_file_write_func pCallback, void *pOpaque, mz_uint flags)
{
    int file_index = mz_zip_reader_locate_file(pZip, pFilename, NULL, flags);
    if (file_index < 0)
        return MZ_FALSE;
    return mz_zip_reader_extract_to_callback(pZip, file_index, pCallback, pOpaque, flags);
}

#ifndef MINIZ_NO_STDIO
static size_t mz_zip_file_write_callback(void *pOpaque, mz_uint64 ofs, const void *pBuf, size_t n)
{
    (void)ofs;
    return MZ_FWRITE(pBuf, 1, n, (MZ_FILE *)pOpaque);
}

mz_bool mz_zip_reader_extract_to_file(mz_zip_archive *pZip, mz_uint file_index, const char *pDst_filename, mz_uint flags)
{
    mz_bool status;
    mz_zip_archive_file_stat file_stat;
    MZ_FILE *pFile;
    if (!mz_zip_reader_file_stat(pZip, file_index, &file_stat))
        return MZ_FALSE;
    pFile = MZ_FOPEN(pDst_filename, "wb");
    if (!pFile)
        return MZ_FALSE;
    status = mz_zip_reader_extract_to_callback(pZip, file_index, mz_zip_file_write_callback, pFile, flags);
    if (MZ_FCLOSE(pFile) == EOF)
        return MZ_FALSE;
#ifndef MINIZ_NO_TIME
    if (status)
        mz_zip_set_file_times(pDst_filename, file_stat.m_time, file_stat.m_time);
#endif
    return status;
}
#endif // #ifndef MINIZ_NO_STDIO

mz_bool mz_zip_reader_end(mz_zip_archive *pZip)
{
    if ((!pZip) || (!pZip->m_pState) || (!pZip->m_pAlloc) || (!pZip->m_pFree) || (pZip->m_zip_mode != MZ_ZIP_MODE_READING))
        return MZ_FALSE;

    if (pZip->m_pState)
    {
        mz_zip_internal_state *pState = pZip->m_pState;
        pZip->m_pState = NULL;
        mz_zip_array_clear(pZip, &pState->m_central_dir);
        mz_zip_array_clear(pZip, &pState->m_central_dir_offsets);
        mz_zip_array_clear(pZip, &pState->m_sorted_central_dir_offsets);

#ifndef MINIZ_NO_STDIO
        if (pState->m_pFile)
        {
            MZ_FCLOSE(pState->m_pFile);
            pState->m_pFile = NULL;
        }
#endif // #ifndef MINIZ_NO_STDIO

        pZip->m_pFree(pZip->m_pAlloc_opaque, pState);
    }
    pZip->m_zip_mode = MZ_ZIP_MODE_INVALID;

    return MZ_TRUE;
}

#ifndef MINIZ_NO_STDIO
mz_bool mz_zip_reader_extract_file_to_file(mz_zip_archive *pZip, const char *pArchive_filename, const char *pDst_filename, mz_uint flags)
{
    int file_index = mz_zip_reader_locate_file(pZip, pArchive_filename, NULL, flags);
    if (file_index < 0)
        return MZ_FALSE;
    return mz_zip_reader_extract_to_file(pZip, file_index, pDst_filename, flags);
}
#endif

// ------------------- .ZIP archive writing

#ifndef MINIZ_NO_ARCHIVE_WRITING_APIS

static void mz_write_le16(mz_uint8 *p, mz_uint16 v)
{
    p[0] = (mz_uint8)v;
    p[1] = (mz_uint8)(v >> 8);
}
static void mz_write_le32(mz_uint8 *p, mz_uint32 v)
{
    p[0] = (mz_uint8)v;
    p[1] = (mz_uint8)(v >> 8);
    p[2] = (mz_uint8)(v >> 16);
    p[3] = (mz_uint8)(v >> 24);
}
#define MZ_WRITE_LE16(p, v) mz_write_le16((mz_uint8 *)(p), (mz_uint16)(v))
#define MZ_WRITE_LE32(p, v) mz_write_le32((mz_uint8 *)(p), (mz_uint32)(v))

mz_bool mz_zip_writer_init(mz_zip_archive *pZip, mz_uint64 existing_size)
{
    if ((!pZip) || (pZip->m_pState) || (!pZip->m_pWrite) || (pZip->m_zip_mode != MZ_ZIP_MODE_INVALID))
        return MZ_FALSE;

    if (pZip->m_file_offset_alignment)
    {
        // Ensure user specified file offset alignment is a power of 2.
        if (pZip->m_file_offset_alignment & (pZip->m_file_offset_alignment - 1))
            return MZ_FALSE;
    }

    if (!pZip->m_pAlloc)
        pZip->m_pAlloc = miniz_def_alloc_func;
    if (!pZip->m_pFree)
        pZip->m_pFree = miniz_def_free_func;
    if (!pZip->m_pRealloc)
        pZip->m_pRealloc = miniz_def_realloc_func;

    pZip->m_zip_mode = MZ_ZIP_MODE_WRITING;
    pZip->m_archive_size = existing_size;
    pZip->m_central_directory_file_ofs = 0;
    pZip->m_total_files = 0;

    if (NULL == (pZip->m_pState = (mz_zip_internal_state *)pZip->m_pAlloc(pZip->m_pAlloc_opaque, 1, sizeof(mz_zip_internal_state))))
        return MZ_FALSE;
    memset(pZip->m_pState, 0, sizeof(mz_zip_internal_state));
    MZ_ZIP_ARRAY_SET_ELEMENT_SIZE(&pZip->m_pState->m_central_dir, sizeof(mz_uint8));
    MZ_ZIP_ARRAY_SET_ELEMENT_SIZE(&pZip->m_pState->m_central_dir_offsets, sizeof(mz_uint32));
    MZ_ZIP_ARRAY_SET_ELEMENT_SIZE(&pZip->m_pState->m_sorted_central_dir_offsets, sizeof(mz_uint32));
    return MZ_TRUE;
}

static size_t mz_zip_heap_write_func(void *pOpaque, mz_uint64 file_ofs, const void *pBuf, size_t n)
{
    mz_zip_archive *pZip = (mz_zip_archive *)pOpaque;
    mz_zip_internal_state *pState = pZip->m_pState;
    mz_uint64 new_size = MZ_MAX(file_ofs + n, pState->m_mem_size);
#ifdef _MSC_VER
    if ((!n) || ((0, sizeof(size_t) == sizeof(mz_uint32)) && (new_size > 0x7FFFFFFF)))
#else
    if ((!n) || ((sizeof(size_t) == sizeof(mz_uint32)) && (new_size > 0x7FFFFFFF)))
#endif
        return 0;
    if (new_size > pState->m_mem_capacity)
    {
        void *pNew_block;
        size_t new_capacity = MZ_MAX(64, pState->m_mem_capacity);
        while (new_capacity < new_size)
            new_capacity *= 2;
        if (NULL == (pNew_block = pZip->m_pRealloc(pZip->m_pAlloc_opaque, pState->m_pMem, 1, new_capacity)))
            return 0;
        pState->m_pMem = pNew_block;
        pState->m_mem_capacity = new_capacity;
    }
    memcpy((mz_uint8 *)pState->m_pMem + file_ofs, pBuf, n);
    pState->m_mem_size = (size_t)new_size;
    return n;
}

mz_bool mz_zip_writer_init_heap(mz_zip_archive *pZip, size_t size_to_reserve_at_beginning, size_t initial_allocation_size)
{
    pZip->m_pWrite = mz_zip_heap_write_func;
    pZip->m_pIO_opaque = pZip;
    if (!mz_zip_writer_init(pZip, size_to_reserve_at_beginning))
        return MZ_FALSE;
    if (0 != (initial_allocation_size = MZ_MAX(initial_allocation_size, size_to_reserve_at_beginning)))
    {
        if (NULL == (pZip->m_pState->m_pMem = pZip->m_pAlloc(pZip->m_pAlloc_opaque, 1, initial_allocation_size)))
        {
            mz_zip_writer_end(pZip);
            return MZ_FALSE;
        }
        pZip->m_pState->m_mem_capacity = initial_allocation_size;
    }
    return MZ_TRUE;
}

#ifndef MINIZ_NO_STDIO
static size_t mz_zip_file_write_func(void *pOpaque, mz_uint64 file_ofs, const void *pBuf, size_t n)
{
    mz_zip_archive *pZip = (mz_zip_archive *)pOpaque;
    mz_int64 cur_ofs = MZ_FTELL64(pZip->m_pState->m_pFile);
    if (((mz_int64)file_ofs < 0) || (((cur_ofs != (mz_int64)file_ofs)) && (MZ_FSEEK64(pZip->m_pState->m_pFile, (mz_int64)file_ofs, SEEK_SET))))
        return 0;
    return MZ_FWRITE(pBuf, 1, n, pZip->m_pState->m_pFile);
}

mz_bool mz_zip_writer_init_file(mz_zip_archive *pZip, const char *pFilename, mz_uint64 size_to_reserve_at_beginning)
{
    MZ_FILE *pFile;
    pZip->m_pWrite = mz_zip_file_write_func;
    pZip->m_pIO_opaque = pZip;
    if (!mz_zip_writer_init(pZip, size_to_reserve_at_beginning))
        return MZ_FALSE;
    if (NULL == (pFile = MZ_FOPEN(pFilename, "wb")))
    {
        mz_zip_writer_end(pZip);
        return MZ_FALSE;
    }
    pZip->m_pState->m_pFile = pFile;
    if (size_to_reserve_at_beginning)
    {
        mz_uint64 cur_ofs = 0;
        char buf[4096];
        MZ_CLEAR_OBJ(buf);
        do
        {
            size_t n = (size_t)MZ_MIN(sizeof(buf), size_to_reserve_at_beginning);
            if (pZip->m_pWrite(pZip->m_pIO_opaque, cur_ofs, buf, n) != n)
            {
                mz_zip_writer_end(pZip);
                return MZ_FALSE;
            }
            cur_ofs += n;
            size_to_reserve_at_beginning -= n;
        } while (size_to_reserve_at_beginning);
    }
    return MZ_TRUE;
}
#endif // #ifndef MINIZ_NO_STDIO

mz_bool mz_zip_writer_init_from_reader(mz_zip_archive *pZip, const char *pFilename)
{
    mz_zip_internal_state *pState;
    if ((!pZip) || (!pZip->m_pState) || (pZip->m_zip_mode != MZ_ZIP_MODE_READING))
        return MZ_FALSE;
    // No sense in trying to write to an archive that's already at the support max size
    if ((pZip->m_total_files == 0xFFFF) || ((pZip->m_archive_size + MZ_ZIP_CENTRAL_DIR_HEADER_SIZE + MZ_ZIP_LOCAL_DIR_HEADER_SIZE) > 0xFFFFFFFF))
        return MZ_FALSE;

    pState = pZip->m_pState;

    if (pState->m_pFile)
    {
#ifdef MINIZ_NO_STDIO
        pFilename;
        return MZ_FALSE;
#else
        // Archive is being read from stdio - try to reopen as writable.
        if (pZip->m_pIO_opaque != pZip)
            return MZ_FALSE;
        if (!pFilename)
            return MZ_FALSE;
        pZip->m_pWrite = mz_zip_file_write_func;
        if (NULL == (pState->m_pFile = MZ_FREOPEN(pFilename, "r+b", pState->m_pFile)))
        {
            // The mz_zip_archive is now in a bogus state because pState->m_pFile is NULL, so just close it.
            mz_zip_reader_end(pZip);
            return MZ_FALSE;
        }
#endif // #ifdef MINIZ_NO_STDIO
    }
    else if (pState->m_pMem)
    {
        // Archive lives in a memory block. Assume it's from the heap that we can resize using the realloc callback.
        if (pZip->m_pIO_opaque != pZip)
            return MZ_FALSE;
        pState->m_mem_capacity = pState->m_mem_size;
        pZip->m_pWrite = mz_zip_heap_write_func;
    }
    // Archive is being read via a user provided read function - make sure the user has specified a write function too.
    else if (!pZip->m_pWrite)
        return MZ_FALSE;

    // Start writing new files at the archive's current central directory location.
    pZip->m_archive_size = pZip->m_central_directory_file_ofs;
    pZip->m_zip_mode = MZ_ZIP_MODE_WRITING;
    pZip->m_central_directory_file_ofs = 0;

    return MZ_TRUE;
}

mz_bool mz_zip_writer_add_mem(mz_zip_archive *pZip, const char *pArchive_name, const void *pBuf, size_t buf_size, mz_uint level_and_flags)
{
    return mz_zip_writer_add_mem_ex(pZip, pArchive_name, pBuf, buf_size, NULL, 0, level_and_flags, 0, 0);
}

typedef struct
{
    mz_zip_archive *m_pZip;
    mz_uint64 m_cur_archive_file_ofs;
    mz_uint64 m_comp_size;
} mz_zip_writer_add_state;

static mz_bool mz_zip_writer_add_put_buf_callback(const void *pBuf, int len, void *pUser)
{
    mz_zip_writer_add_state *pState = (mz_zip_writer_add_state *)pUser;
    if ((int)pState->m_pZip->m_pWrite(pState->m_pZip->m_pIO_opaque, pState->m_cur_archive_file_ofs, pBuf, len) != len)
        return MZ_FALSE;
    pState->m_cur_archive_file_ofs += len;
    pState->m_comp_size += len;
    return MZ_TRUE;
}

static mz_bool mz_zip_writer_create_local_dir_header(mz_zip_archive *pZip, mz_uint8 *pDst, mz_uint16 filename_size, mz_uint16 extra_size, mz_uint64 uncomp_size, mz_uint64 comp_size, mz_uint32 uncomp_crc32, mz_uint16 method, mz_uint16 bit_flags, mz_uint16 dos_time, mz_uint16 dos_date)
{
    (void)pZip;
    memset(pDst, 0, MZ_ZIP_LOCAL_DIR_HEADER_SIZE);
    MZ_WRITE_LE32(pDst + MZ_ZIP_LDH_SIG_OFS, MZ_ZIP_LOCAL_DIR_HEADER_SIG);
    MZ_WRITE_LE16(pDst + MZ_ZIP_LDH_VERSION_NEEDED_OFS, method ? 20 : 0);
    MZ_WRITE_LE16(pDst + MZ_ZIP_LDH_BIT_FLAG_OFS, bit_flags);
    MZ_WRITE_LE16(pDst + MZ_ZIP_LDH_METHOD_OFS, method);
    MZ_WRITE_LE16(pDst + MZ_ZIP_LDH_FILE_TIME_OFS, dos_time);
    MZ_WRITE_LE16(pDst + MZ_ZIP_LDH_FILE_DATE_OFS, dos_date);
    MZ_WRITE_LE32(pDst + MZ_ZIP_LDH_CRC32_OFS, uncomp_crc32);
    MZ_WRITE_LE32(pDst + MZ_ZIP_LDH_COMPRESSED_SIZE_OFS, comp_size);
    MZ_WRITE_LE32(pDst + MZ_ZIP_LDH_DECOMPRESSED_SIZE_OFS, uncomp_size);
    MZ_WRITE_LE16(pDst + MZ_ZIP_LDH_FILENAME_LEN_OFS, filename_size);
    MZ_WRITE_LE16(pDst + MZ_ZIP_LDH_EXTRA_LEN_OFS, extra_size);
    return MZ_TRUE;
}

static mz_bool mz_zip_writer_create_central_dir_header(mz_zip_archive *pZip, mz_uint8 *pDst, mz_uint16 filename_size, mz_uint16 extra_size, mz_uint16 comment_size, mz_uint64 uncomp_size, mz_uint64 comp_size, mz_uint32 uncomp_crc32, mz_uint16 method, mz_uint16 bit_flags, mz_uint16 dos_time, mz_uint16 dos_date, mz_uint64 local_header_ofs, mz_uint32 ext_attributes)
{
    (void)pZip;
    memset(pDst, 0, MZ_ZIP_CENTRAL_DIR_HEADER_SIZE);
    MZ_WRITE_LE32(pDst + MZ_ZIP_CDH_SIG_OFS, MZ_ZIP_CENTRAL_DIR_HEADER_SIG);
    MZ_WRITE_LE16(pDst + MZ_ZIP_CDH_VERSION_NEEDED_OFS, method ? 20 : 0);
    MZ_WRITE_LE16(pDst + MZ_ZIP_CDH_BIT_FLAG_OFS, bit_flags);
    MZ_WRITE_LE16(pDst + MZ_ZIP_CDH_METHOD_OFS, method);
    MZ_WRITE_LE16(pDst + MZ_ZIP_CDH_FILE_TIME_OFS, dos_time);
    MZ_WRITE_LE16(pDst + MZ_ZIP_CDH_FILE_DATE_OFS, dos_date);
    MZ_WRITE_LE32(pDst + MZ_ZIP_CDH_CRC32_OFS, uncomp_crc32);
    MZ_WRITE_LE32(pDst + MZ_ZIP_CDH_COMPRESSED_SIZE_OFS, comp_size);
    MZ_WRITE_LE32(pDst + MZ_ZIP_CDH_DECOMPRESSED_SIZE_OFS, uncomp_size);
    MZ_WRITE_LE16(pDst + MZ_ZIP_CDH_FILENAME_LEN_OFS, filename_size);
    MZ_WRITE_LE16(pDst + MZ_ZIP_CDH_EXTRA_LEN_OFS, extra_size);
    MZ_WRITE_LE16(pDst + MZ_ZIP_CDH_COMMENT_LEN_OFS, comment_size);
    MZ_WRITE_LE32(pDst + MZ_ZIP_CDH_EXTERNAL_ATTR_OFS, ext_attributes);
    MZ_WRITE_LE32(pDst + MZ_ZIP_CDH_LOCAL_HEADER_OFS, local_header_ofs);
    return MZ_TRUE;
}

static mz_bool mz_zip_writer_add_to_central_dir(mz_zip_archive *pZip, const char *pFilename, mz_uint16 filename_size, const void *pExtra, mz_uint16 extra_size, const void *pComment, mz_uint16 comment_size, mz_uint64 uncomp_size, mz_uint64 comp_size, mz_uint32 uncomp_crc32, mz_uint16 method, mz_uint16 bit_flags, mz_uint16 dos_time, mz_uint16 dos_date, mz_uint64 local_header_ofs, mz_uint32 ext_attributes)
{
    mz_zip_internal_state *pState = pZip->m_pState;
    mz_uint32 central_dir_ofs = (mz_uint32)pState->m_central_dir.m_size;
    size_t orig_central_dir_size = pState->m_central_dir.m_size;
    mz_uint8 central_dir_header[MZ_ZIP_CENTRAL_DIR_HEADER_SIZE];

    // No zip64 support yet
    if ((local_header_ofs > 0xFFFFFFFF) || (((mz_uint64)pState->m_central_dir.m_size + MZ_ZIP_CENTRAL_DIR_HEADER_SIZE + filename_size + extra_size + comment_size) > 0xFFFFFFFF))
        return MZ_FALSE;

    if (!mz_zip_writer_create_central_dir_header(pZip, central_dir_header, filename_size, extra_size, comment_size, uncomp_size, comp_size, uncomp_crc32, method, bit_flags, dos_time, dos_date, local_header_ofs, ext_attributes))
        return MZ_FALSE;

    if ((!mz_zip_array_push_back(pZip, &pState->m_central_dir, central_dir_header, MZ_ZIP_CENTRAL_DIR_HEADER_SIZE)) ||
        (!mz_zip_array_push_back(pZip, &pState->m_central_dir, pFilename, filename_size)) ||
        (!mz_zip_array_push_back(pZip, &pState->m_central_dir, pExtra, extra_size)) ||
        (!mz_zip_array_push_back(pZip, &pState->m_central_dir, pComment, comment_size)) ||
        (!mz_zip_array_push_back(pZip, &pState->m_central_dir_offsets, &central_dir_ofs, 1)))
    {
        // Try to push the central directory array back into its original state.
        mz_zip_array_resize(pZip, &pState->m_central_dir, orig_central_dir_size, MZ_FALSE);
        return MZ_FALSE;
    }

    return MZ_TRUE;
}

static mz_bool mz_zip_writer_validate_archive_name(const char *pArchive_name)
{
    // Basic ZIP archive filename validity checks: Valid filenames cannot start with a forward slash, cannot contain a drive letter, and cannot use DOS-style backward slashes.
    if (*pArchive_name == '/')
        return MZ_FALSE;
    while (*pArchive_name)
    {
        if ((*pArchive_name == '\\') || (*pArchive_name == ':'))
            return MZ_FALSE;
        pArchive_name++;
    }
    return MZ_TRUE;
}

static mz_uint mz_zip_writer_compute_padding_needed_for_file_alignment(mz_zip_archive *pZip)
{
    mz_uint32 n;
    if (!pZip->m_file_offset_alignment)
        return 0;
    n = (mz_uint32)(pZip->m_archive_size & (pZip->m_file_offset_alignment - 1));
    return (pZip->m_file_offset_alignment - n) & (pZip->m_file_offset_alignment - 1);
}

static mz_bool mz_zip_writer_write_zeros(mz_zip_archive *pZip, mz_uint64 cur_file_ofs, mz_uint32 n)
{
    char buf[4096];
    memset(buf, 0, MZ_MIN(sizeof(buf), n));
    while (n)
    {
        mz_uint32 s = MZ_MIN(sizeof(buf), n);
        if (pZip->m_pWrite(pZip->m_pIO_opaque, cur_file_ofs, buf, s) != s)
            return MZ_FALSE;
        cur_file_ofs += s;
        n -= s;
    }
    return MZ_TRUE;
}

mz_bool mz_zip_writer_add_mem_ex(mz_zip_archive *pZip, const char *pArchive_name, const void *pBuf, size_t buf_size, const void *pComment, mz_uint16 comment_size, mz_uint level_and_flags, mz_uint64 uncomp_size, mz_uint32 uncomp_crc32)
{
    mz_uint16 method = 0, dos_time = 0, dos_date = 0;
    mz_uint level, ext_attributes = 0, num_alignment_padding_bytes;
    mz_uint64 local_dir_header_ofs = pZip->m_archive_size, cur_archive_file_ofs = pZip->m_archive_size, comp_size = 0;
    size_t archive_name_size;
    mz_uint8 local_dir_header[MZ_ZIP_LOCAL_DIR_HEADER_SIZE];
    tdefl_compressor *pComp = NULL;
    mz_bool store_data_uncompressed;
    mz_zip_internal_state *pState;

    if ((int)level_and_flags < 0)
        level_and_flags = MZ_DEFAULT_LEVEL;
    level = level_and_flags & 0xF;
    store_data_uncompressed = ((!level) || (level_and_flags & MZ_ZIP_FLAG_COMPRESSED_DATA));

    if ((!pZip) || (!pZip->m_pState) || (pZip->m_zip_mode != MZ_ZIP_MODE_WRITING) || ((buf_size) && (!pBuf)) || (!pArchive_name) || ((comment_size) && (!pComment)) || (pZip->m_total_files == 0xFFFF) || (level > MZ_UBER_COMPRESSION))
        return MZ_FALSE;

    pState = pZip->m_pState;

    if ((!(level_and_flags & MZ_ZIP_FLAG_COMPRESSED_DATA)) && (uncomp_size))
        return MZ_FALSE;
    // No zip64 support yet
    if ((buf_size > 0xFFFFFFFF) || (uncomp_size > 0xFFFFFFFF))
        return MZ_FALSE;
    if (!mz_zip_writer_validate_archive_name(pArchive_name))
        return MZ_FALSE;

#ifndef MINIZ_NO_TIME
    {
        time_t cur_time;
        time(&cur_time);
        mz_zip_time_to_dos_time(cur_time, &dos_time, &dos_date);
    }
#endif // #ifndef MINIZ_NO_TIME

    archive_name_size = strlen(pArchive_name);
    if (archive_name_size > 0xFFFF)
        return MZ_FALSE;

    num_alignment_padding_bytes = mz_zip_writer_compute_padding_needed_for_file_alignment(pZip);

    // no zip64 support yet
    if ((pZip->m_total_files == 0xFFFF) || ((pZip->m_archive_size + num_alignment_padding_bytes + MZ_ZIP_LOCAL_DIR_HEADER_SIZE + MZ_ZIP_CENTRAL_DIR_HEADER_SIZE + comment_size + archive_name_size) > 0xFFFFFFFF))
        return MZ_FALSE;

    if ((archive_name_size) && (pArchive_name[archive_name_size - 1] == '/'))
    {
        // Set DOS Subdirectory attribute bit.
        ext_attributes |= 0x10;
        // Subdirectories cannot contain data.
        if ((buf_size) || (uncomp_size))
            return MZ_FALSE;
    }

    // Try to do any allocations before writing to the archive, so if an allocation fails the file remains unmodified. (A good idea if we're doing an in-place modification.)
    if ((!mz_zip_array_ensure_room(pZip, &pState->m_central_dir, MZ_ZIP_CENTRAL_DIR_HEADER_SIZE + archive_name_size + comment_size)) || (!mz_zip_array_ensure_room(pZip, &pState->m_central_dir_offsets, 1)))
        return MZ_FALSE;

    if ((!store_data_uncompressed) && (buf_size))
    {
        if (NULL == (pComp = (tdefl_compressor *)pZip->m_pAlloc(pZip->m_pAlloc_opaque, 1, sizeof(tdefl_compressor))))
            return MZ_FALSE;
    }

    if (!mz_zip_writer_write_zeros(pZip, cur_archive_file_ofs, num_alignment_padding_bytes + sizeof(local_dir_header)))
    {
        pZip->m_pFree(pZip->m_pAlloc_opaque, pComp);
        return MZ_FALSE;
    }
    local_dir_header_ofs += num_alignment_padding_bytes;
    if (pZip->m_file_offset_alignment)
    {
        MZ_ASSERT((local_dir_header_ofs & (pZip->m_file_offset_alignment - 1)) == 0);
    }
    cur_archive_file_ofs += num_alignment_padding_bytes + sizeof(local_dir_header);

    MZ_CLEAR_OBJ(local_dir_header);
    if (pZip->m_pWrite(pZip->m_pIO_opaque, cur_archive_file_ofs, pArchive_name, archive_name_size) != archive_name_size)
    {
        pZip->m_pFree(pZip->m_pAlloc_opaque, pComp);
        return MZ_FALSE;
    }
    cur_archive_file_ofs += archive_name_size;

    if (!(level_and_flags & MZ_ZIP_FLAG_COMPRESSED_DATA))
    {
        uncomp_crc32 = (mz_uint32)mz_crc32(MZ_CRC32_INIT, (const mz_uint8 *)pBuf, buf_size);
        uncomp_size = buf_size;
        if (uncomp_size <= 3)
        {
            level = 0;
            store_data_uncompressed = MZ_TRUE;
        }
    }

    if (store_data_uncompressed)
    {
        if (pZip->m_pWrite(pZip->m_pIO_opaque, cur_archive_file_ofs, pBuf, buf_size) != buf_size)
        {
            pZip->m_pFree(pZip->m_pAlloc_opaque, pComp);
            return MZ_FALSE;
        }

        cur_archive_file_ofs += buf_size;
        comp_size = buf_size;

        if (level_and_flags & MZ_ZIP_FLAG_COMPRESSED_DATA)
            method = MZ_DEFLATED;
    }
    else if (buf_size)
    {
        mz_zip_writer_add_state state;

        state.m_pZip = pZip;
        state.m_cur_archive_file_ofs = cur_archive_file_ofs;
        state.m_comp_size = 0;

        if ((tdefl_init(pComp, mz_zip_writer_add_put_buf_callback, &state, tdefl_create_comp_flags_from_zip_params(level, -15, MZ_DEFAULT_STRATEGY)) != TDEFL_STATUS_OKAY) ||
            (tdefl_compress_buffer(pComp, pBuf, buf_size, TDEFL_FINISH) != TDEFL_STATUS_DONE))
        {
            pZip->m_pFree(pZip->m_pAlloc_opaque, pComp);
            return MZ_FALSE;
        }

        comp_size = state.m_comp_size;
        cur_archive_file_ofs = state.m_cur_archive_file_ofs;

        method = MZ_DEFLATED;
    }

    pZip->m_pFree(pZip->m_pAlloc_opaque, pComp);
    pComp = NULL;

    // no zip64 support yet
    if ((comp_size > 0xFFFFFFFF) || (cur_archive_file_ofs > 0xFFFFFFFF))
        return MZ_FALSE;

    if (!mz_zip_writer_create_local_dir_header(pZip, local_dir_header, (mz_uint16)archive_name_size, 0, uncomp_size, comp_size, uncomp_crc32, method, 0, dos_time, dos_date))
        return MZ_FALSE;

    if (pZip->m_pWrite(pZip->m_pIO_opaque, local_dir_header_ofs, local_dir_header, sizeof(local_dir_header)) != sizeof(local_dir_header))
        return MZ_FALSE;

    if (!mz_zip_writer_add_to_central_dir(pZip, pArchive_name, (mz_uint16)archive_name_size, NULL, 0, pComment, comment_size, uncomp_size, comp_size, uncomp_crc32, method, 0, dos_time, dos_date, local_dir_header_ofs, ext_attributes))
        return MZ_FALSE;

    pZip->m_total_files++;
    pZip->m_archive_size = cur_archive_file_ofs;

    return MZ_TRUE;
}

#ifndef MINIZ_NO_STDIO
mz_bool mz_zip_writer_add_file(mz_zip_archive *pZip, const char *pArchive_name, const char *pSrc_filename, const void *pComment, mz_uint16 comment_size, mz_uint level_and_flags)
{
    mz_uint uncomp_crc32 = MZ_CRC32_INIT, level, num_alignment_padding_bytes;
    mz_uint16 method = 0, dos_time = 0, dos_date = 0, ext_attributes = 0;
    mz_uint64 local_dir_header_ofs = pZip->m_archive_size, cur_archive_file_ofs = pZip->m_archive_size, uncomp_size = 0, comp_size = 0;
    size_t archive_name_size;
    mz_uint8 local_dir_header[MZ_ZIP_LOCAL_DIR_HEADER_SIZE];
    MZ_FILE *pSrc_file = NULL;

    if ((int)level_and_flags < 0)
        level_and_flags = MZ_DEFAULT_LEVEL;
    level = level_and_flags & 0xF;

    if ((!pZip) || (!pZip->m_pState) || (pZip->m_zip_mode != MZ_ZIP_MODE_WRITING) || (!pArchive_name) || ((comment_size) && (!pComment)) || (level > MZ_UBER_COMPRESSION))
        return MZ_FALSE;
    if (level_and_flags & MZ_ZIP_FLAG_COMPRESSED_DATA)
        return MZ_FALSE;
    if (!mz_zip_writer_validate_archive_name(pArchive_name))
        return MZ_FALSE;

    archive_name_size = strlen(pArchive_name);
    if (archive_name_size > 0xFFFF)
        return MZ_FALSE;

    num_alignment_padding_bytes = mz_zip_writer_compute_padding_needed_for_file_alignment(pZip);

    // no zip64 support yet
    if ((pZip->m_total_files == 0xFFFF) || ((pZip->m_archive_size + num_alignment_padding_bytes + MZ_ZIP_LOCAL_DIR_HEADER_SIZE + MZ_ZIP_CENTRAL_DIR_HEADER_SIZE + comment_size + archive_name_size) > 0xFFFFFFFF))
        return MZ_FALSE;

    if (!mz_zip_get_file_modified_time(pSrc_filename, &dos_time, &dos_date))
        return MZ_FALSE;

    pSrc_file = MZ_FOPEN(pSrc_filename, "rb");
    if (!pSrc_file)
        return MZ_FALSE;
    MZ_FSEEK64(pSrc_file, 0, SEEK_END);
    uncomp_size = MZ_FTELL64(pSrc_file);
    MZ_FSEEK64(pSrc_file, 0, SEEK_SET);

    if (uncomp_size > 0xFFFFFFFF)
    {
        // No zip64 support yet
        MZ_FCLOSE(pSrc_file);
        return MZ_FALSE;
    }
    if (uncomp_size <= 3)
        level = 0;

    if (!mz_zip_writer_write_zeros(pZip, cur_archive_file_ofs, num_alignment_padding_bytes + sizeof(local_dir_header)))
    {
        MZ_FCLOSE(pSrc_file);
        return MZ_FALSE;
    }
    local_dir_header_ofs += num_alignment_padding_bytes;
    if (pZip->m_file_offset_alignment)
    {
        MZ_ASSERT((local_dir_header_ofs & (pZip->m_file_offset_alignment - 1)) == 0);
    }
    cur_archive_file_ofs += num_alignment_padding_bytes + sizeof(local_dir_header);

    MZ_CLEAR_OBJ(local_dir_header);
    if (pZip->m_pWrite(pZip->m_pIO_opaque, cur_archive_file_ofs, pArchive_name, archive_name_size) != archive_name_size)
    {
        MZ_FCLOSE(pSrc_file);
        return MZ_FALSE;
    }
    cur_archive_file_ofs += archive_name_size;

    if (uncomp_size)
    {
        mz_uint64 uncomp_remaining = uncomp_size;
        void *pRead_buf = pZip->m_pAlloc(pZip->m_pAlloc_opaque, 1, MZ_ZIP_MAX_IO_BUF_SIZE);
        if (!pRead_buf)
        {
            MZ_FCLOSE(pSrc_file);
            return MZ_FALSE;
        }

        if (!level)
        {
            while (uncomp_remaining)
            {
                mz_uint n = (mz_uint)MZ_MIN(MZ_ZIP_MAX_IO_BUF_SIZE, uncomp_remaining);
                if ((MZ_FREAD(pRead_buf, 1, n, pSrc_file) != n) || (pZip->m_pWrite(pZip->m_pIO_opaque, cur_archive_file_ofs, pRead_buf, n) != n))
                {
                    pZip->m_pFree(pZip->m_pAlloc_opaque, pRead_buf);
                    MZ_FCLOSE(pSrc_file);
                    return MZ_FALSE;
                }
                uncomp_crc32 = (mz_uint32)mz_crc32(uncomp_crc32, (const mz_uint8 *)pRead_buf, n);
                uncomp_remaining -= n;
                cur_archive_file_ofs += n;
            }
            comp_size = uncomp_size;
        }
        else
        {
            mz_bool result = MZ_FALSE;
            mz_zip_writer_add_state state;
            tdefl_compressor *pComp = (tdefl_compressor *)pZip->m_pAlloc(pZip->m_pAlloc_opaque, 1, sizeof(tdefl_compressor));
            if (!pComp)
            {
                pZip->m_pFree(pZip->m_pAlloc_opaque, pRead_buf);
                MZ_FCLOSE(pSrc_file);
                return MZ_FALSE;
            }

            state.m_pZip = pZip;
            state.m_cur_archive_file_ofs = cur_archive_file_ofs;
            state.m_comp_size = 0;

            if (tdefl_init(pComp, mz_zip_writer_add_put_buf_callback, &state, tdefl_create_comp_flags_from_zip_params(level, -15, MZ_DEFAULT_STRATEGY)) != TDEFL_STATUS_OKAY)
            {
                pZip->m_pFree(pZip->m_pAlloc_opaque, pComp);
                pZip->m_pFree(pZip->m_pAlloc_opaque, pRead_buf);
                MZ_FCLOSE(pSrc_file);
                return MZ_FALSE;
            }

            for (;;)
            {
                size_t in_buf_size = (mz_uint32)MZ_MIN(uncomp_remaining, MZ_ZIP_MAX_IO_BUF_SIZE);
                tdefl_status status;

                if (MZ_FREAD(pRead_buf, 1, in_buf_size, pSrc_file) != in_buf_size)
                    break;

                uncomp_crc32 = (mz_uint32)mz_crc32(uncomp_crc32, (const mz_uint8 *)pRead_buf, in_buf_size);
                uncomp_remaining -= in_buf_size;

                status = tdefl_compress_buffer(pComp, pRead_buf, in_buf_size, uncomp_remaining ? TDEFL_NO_FLUSH : TDEFL_FINISH);
                if (status == TDEFL_STATUS_DONE)
                {
                    result = MZ_TRUE;
                    break;
                }
                else if (status != TDEFL_STATUS_OKAY)
                    break;
            }

            pZip->m_pFree(pZip->m_pAlloc_opaque, pComp);

            if (!result)
            {
                pZip->m_pFree(pZip->m_pAlloc_opaque, pRead_buf);
                MZ_FCLOSE(pSrc_file);
                return MZ_FALSE;
            }

            comp_size = state.m_comp_size;
            cur_archive_file_ofs = state.m_cur_archive_file_ofs;

            method = MZ_DEFLATED;
        }

        pZip->m_pFree(pZip->m_pAlloc_opaque, pRead_buf);
    }

    MZ_FCLOSE(pSrc_file);
    pSrc_file = NULL;

    // no zip64 support yet
    if ((comp_size > 0xFFFFFFFF) || (cur_archive_file_ofs > 0xFFFFFFFF))
        return MZ_FALSE;

    if (!mz_zip_writer_create_local_dir_header(pZip, local_dir_header, (mz_uint16)archive_name_size, 0, uncomp_size, comp_size, uncomp_crc32, method, 0, dos_time, dos_date))
        return MZ_FALSE;

    if (pZip->m_pWrite(pZip->m_pIO_opaque, local_dir_header_ofs, local_dir_header, sizeof(local_dir_header)) != sizeof(local_dir_header))
        return MZ_FALSE;

    if (!mz_zip_writer_add_to_central_dir(pZip, pArchive_name, (mz_uint16)archive_name_size, NULL, 0, pComment, comment_size, uncomp_size, comp_size, uncomp_crc32, method, 0, dos_time, dos_date, local_dir_header_ofs, ext_attributes))
        return MZ_FALSE;

    pZip->m_total_files++;
    pZip->m_archive_size = cur_archive_file_ofs;

    return MZ_TRUE;
}
#endif // #ifndef MINIZ_NO_STDIO

mz_bool mz_zip_writer_add_from_zip_reader(mz_zip_archive *pZip, mz_zip_archive *pSource_zip, mz_uint file_index)
{
    mz_uint n, bit_flags, num_alignment_padding_bytes;
    mz_uint64 comp_bytes_remaining, local_dir_header_ofs;
    mz_uint64 cur_src_file_ofs, cur_dst_file_ofs;
    mz_uint32 local_header_u32[(MZ_ZIP_LOCAL_DIR_HEADER_SIZE + sizeof(mz_uint32) - 1) / sizeof(mz_uint32)];
    mz_uint8 *pLocal_header = (mz_uint8 *)local_header_u32;
    mz_uint8 central_header[MZ_ZIP_CENTRAL_DIR_HEADER_SIZE];
    size_t orig_central_dir_size;
    mz_zip_internal_state *pState;
    void *pBuf;
    const mz_uint8 *pSrc_central_header;

    if ((!pZip) || (!pZip->m_pState) || (pZip->m_zip_mode != MZ_ZIP_MODE_WRITING))
        return MZ_FALSE;
    if (NULL == (pSrc_central_header = mz_zip_reader_get_cdh(pSource_zip, file_index)))
        return MZ_FALSE;
    pState = pZip->m_pState;

    num_alignment_padding_bytes = mz_zip_writer_compute_padding_needed_for_file_alignment(pZip);

    // no zip64 support yet
    if ((pZip->m_total_files == 0xFFFF) || ((pZip->m_archive_size + num_alignment_padding_bytes + MZ_ZIP_LOCAL_DIR_HEADER_SIZE + MZ_ZIP_CENTRAL_DIR_HEADER_SIZE) > 0xFFFFFFFF))
        return MZ_FALSE;

    cur_src_file_ofs = MZ_READ_LE32(pSrc_central_header + MZ_ZIP_CDH_LOCAL_HEADER_OFS);
    cur_dst_file_ofs = pZip->m_archive_size;

    if (pSource_zip->m_pRead(pSource_zip->m_pIO_opaque, cur_src_file_ofs, pLocal_header, MZ_ZIP_LOCAL_DIR_HEADER_SIZE) != MZ_ZIP_LOCAL_DIR_HEADER_SIZE)
        return MZ_FALSE;
    if (MZ_READ_LE32(pLocal_header) != MZ_ZIP_LOCAL_DIR_HEADER_SIG)
        return MZ_FALSE;
    cur_src_file_ofs += MZ_ZIP_LOCAL_DIR_HEADER_SIZE;

    if (!mz_zip_writer_write_zeros(pZip, cur_dst_file_ofs, num_alignment_padding_bytes))
        return MZ_FALSE;
    cur_dst_file_ofs += num_alignment_padding_bytes;
    local_dir_header_ofs = cur_dst_file_ofs;
    if (pZip->m_file_offset_alignment)
    {
        MZ_ASSERT((local_dir_header_ofs & (pZip->m_file_offset_alignment - 1)) == 0);
    }

    if (pZip->m_pWrite(pZip->m_pIO_opaque, cur_dst_file_ofs, pLocal_header, MZ_ZIP_LOCAL_DIR_HEADER_SIZE) != MZ_ZIP_LOCAL_DIR_HEADER_SIZE)
        return MZ_FALSE;
    cur_dst_file_ofs += MZ_ZIP_LOCAL_DIR_HEADER_SIZE;

    n = MZ_READ_LE16(pLocal_header + MZ_ZIP_LDH_FILENAME_LEN_OFS) + MZ_READ_LE16(pLocal_header + MZ_ZIP_LDH_EXTRA_LEN_OFS);
    comp_bytes_remaining = n + MZ_READ_LE32(pSrc_central_header + MZ_ZIP_CDH_COMPRESSED_SIZE_OFS);

    if (NULL == (pBuf = pZip->m_pAlloc(pZip->m_pAlloc_opaque, 1, (size_t)MZ_MAX(sizeof(mz_uint32) * 4, MZ_MIN(MZ_ZIP_MAX_IO_BUF_SIZE, comp_bytes_remaining)))))
        return MZ_FALSE;

    while (comp_bytes_remaining)
    {
        n = (mz_uint)MZ_MIN(MZ_ZIP_MAX_IO_BUF_SIZE, comp_bytes_remaining);
        if (pSource_zip->m_pRead(pSource_zip->m_pIO_opaque, cur_src_file_ofs, pBuf, n) != n)
        {
            pZip->m_pFree(pZip->m_pAlloc_opaque, pBuf);
            return MZ_FALSE;
        }
        cur_src_file_ofs += n;

        if (pZip->m_pWrite(pZip->m_pIO_opaque, cur_dst_file_ofs, pBuf, n) != n)
        {
            pZip->m_pFree(pZip->m_pAlloc_opaque, pBuf);
            return MZ_FALSE;
        }
        cur_dst_file_ofs += n;

        comp_bytes_remaining -= n;
    }

    bit_flags = MZ_READ_LE16(pLocal_header + MZ_ZIP_LDH_BIT_FLAG_OFS);
    if (bit_flags & 8)
    {
        // Copy data descriptor
        if (pSource_zip->m_pRead(pSource_zip->m_pIO_opaque, cur_src_file_ofs, pBuf, sizeof(mz_uint32) * 4) != sizeof(mz_uint32) * 4)
        {
            pZip->m_pFree(pZip->m_pAlloc_opaque, pBuf);
            return MZ_FALSE;
        }

        n = sizeof(mz_uint32) * ((MZ_READ_LE32(pBuf) == 0x08074b50) ? 4 : 3);
        if (pZip->m_pWrite(pZip->m_pIO_opaque, cur_dst_file_ofs, pBuf, n) != n)
        {
            pZip->m_pFree(pZip->m_pAlloc_opaque, pBuf);
            return MZ_FALSE;
        }

        cur_src_file_ofs += n;
        cur_dst_file_ofs += n;
    }
    pZip->m_pFree(pZip->m_pAlloc_opaque, pBuf);

    // no zip64 support yet
    if (cur_dst_file_ofs > 0xFFFFFFFF)
        return MZ_FALSE;

    orig_central_dir_size = pState->m_central_dir.m_size;

    memcpy(central_header, pSrc_central_header, MZ_ZIP_CENTRAL_DIR_HEADER_SIZE);
    MZ_WRITE_LE32(central_header + MZ_ZIP_CDH_LOCAL_HEADER_OFS, local_dir_header_ofs);
    if (!mz_zip_array_push_back(pZip, &pState->m_central_dir, central_header, MZ_ZIP_CENTRAL_DIR_HEADER_SIZE))
        return MZ_FALSE;

    n = MZ_READ_LE16(pSrc_central_header + MZ_ZIP_CDH_FILENAME_LEN_OFS) + MZ_READ_LE16(pSrc_central_header + MZ_ZIP_CDH_EXTRA_LEN_OFS) + MZ_READ_LE16(pSrc_central_header + MZ_ZIP_CDH_COMMENT_LEN_OFS);
    if (!mz_zip_array_push_back(pZip, &pState->m_central_dir, pSrc_central_header + MZ_ZIP_CENTRAL_DIR_HEADER_SIZE, n))
    {
        mz_zip_array_resize(pZip, &pState->m_central_dir, orig_central_dir_size, MZ_FALSE);
        return MZ_FALSE;
    }

    if (pState->m_central_dir.m_size > 0xFFFFFFFF)
        return MZ_FALSE;
    n = (mz_uint32)orig_central_dir_size;
    if (!mz_zip_array_push_back(pZip, &pState->m_central_dir_offsets, &n, 1))
    {
        mz_zip_array_resize(pZip, &pState->m_central_dir, orig_central_dir_size, MZ_FALSE);
        return MZ_FALSE;
    }

    pZip->m_total_files++;
    pZip->m_archive_size = cur_dst_file_ofs;

    return MZ_TRUE;
}

mz_bool mz_zip_writer_finalize_archive(mz_zip_archive *pZip)
{
    mz_zip_internal_state *pState;
    mz_uint64 central_dir_ofs, central_dir_size;
    mz_uint8 hdr[MZ_ZIP_END_OF_CENTRAL_DIR_HEADER_SIZE];

    if ((!pZip) || (!pZip->m_pState) || (pZip->m_zip_mode != MZ_ZIP_MODE_WRITING))
        return MZ_FALSE;

    pState = pZip->m_pState;

    // no zip64 support yet
    if ((pZip->m_total_files > 0xFFFF) || ((pZip->m_archive_size + pState->m_central_dir.m_size + MZ_ZIP_END_OF_CENTRAL_DIR_HEADER_SIZE) > 0xFFFFFFFF))
        return MZ_FALSE;

    central_dir_ofs = 0;
    central_dir_size = 0;
    if (pZip->m_total_files)
    {
        // Write central directory
        central_dir_ofs = pZip->m_archive_size;
        central_dir_size = pState->m_central_dir.m_size;
        pZip->m_central_directory_file_ofs = central_dir_ofs;
        if (pZip->m_pWrite(pZip->m_pIO_opaque, central_dir_ofs, pState->m_central_dir.m_p, (size_t)central_dir_size) != central_dir_size)
            return MZ_FALSE;
        pZip->m_archive_size += central_dir_size;
    }

    // Write end of central directory record
    MZ_CLEAR_OBJ(hdr);
    MZ_WRITE_LE32(hdr + MZ_ZIP_ECDH_SIG_OFS, MZ_ZIP_END_OF_CENTRAL_DIR_HEADER_SIG);
    MZ_WRITE_LE16(hdr + MZ_ZIP_ECDH_CDIR_NUM_ENTRIES_ON_DISK_OFS, pZip->m_total_files);
    MZ_WRITE_LE16(hdr + MZ_ZIP_ECDH_CDIR_TOTAL_ENTRIES_OFS, pZip->m_total_files);
    MZ_WRITE_LE32(hdr + MZ_ZIP_ECDH_CDIR_SIZE_OFS, central_dir_size);
    MZ_WRITE_LE32(hdr + MZ_ZIP_ECDH_CDIR_OFS_OFS, central_dir_ofs);

    if (pZip->m_pWrite(pZip->m_pIO_opaque, pZip->m_archive_size, hdr, sizeof(hdr)) != sizeof(hdr))
        return MZ_FALSE;
#ifndef MINIZ_NO_STDIO
    if ((pState->m_pFile) && (MZ_FFLUSH(pState->m_pFile) == EOF))
        return MZ_FALSE;
#endif // #ifndef MINIZ_NO_STDIO

    pZip->m_archive_size += sizeof(hdr);

    pZip->m_zip_mode = MZ_ZIP_MODE_WRITING_HAS_BEEN_FINALIZED;
    return MZ_TRUE;
}

mz_bool mz_zip_writer_finalize_heap_archive(mz_zip_archive *pZip, void **pBuf, size_t *pSize)
{
    if ((!pZip) || (!pZip->m_pState) || (!pBuf) || (!pSize))
        return MZ_FALSE;
    if (pZip->m_pWrite != mz_zip_heap_write_func)
        return MZ_FALSE;
    if (!mz_zip_writer_finalize_archive(pZip))
        return MZ_FALSE;

    *pBuf = pZip->m_pState->m_pMem;
    *pSize = pZip->m_pState->m_mem_size;
    pZip->m_pState->m_pMem = NULL;
    pZip->m_pState->m_mem_size = pZip->m_pState->m_mem_capacity = 0;
    return MZ_TRUE;
}

mz_bool mz_zip_writer_end(mz_zip_archive *pZip)
{
    mz_zip_internal_state *pState;
    mz_bool status = MZ_TRUE;
    if ((!pZip) || (!pZip->m_pState) || (!pZip->m_pAlloc) || (!pZip->m_pFree) || ((pZip->m_zip_mode != MZ_ZIP_MODE_WRITING) && (pZip->m_zip_mode != MZ_ZIP_MODE_WRITING_HAS_BEEN_FINALIZED)))
        return MZ_FALSE;

    pState = pZip->m_pState;
    pZip->m_pState = NULL;
    mz_zip_array_clear(pZip, &pState->m_central_dir);
    mz_zip_array_clear(pZip, &pState->m_central_dir_offsets);
    mz_zip_array_clear(pZip, &pState->m_sorted_central_dir_offsets);

#ifndef MINIZ_NO_STDIO
    if (pState->m_pFile)
    {
        MZ_FCLOSE(pState->m_pFile);
        pState->m_pFile = NULL;
    }
#endif // #ifndef MINIZ_NO_STDIO

    if ((pZip->m_pWrite == mz_zip_heap_write_func) && (pState->m_pMem))
    {
        pZip->m_pFree(pZip->m_pAlloc_opaque, pState->m_pMem);
        pState->m_pMem = NULL;
    }

    pZip->m_pFree(pZip->m_pAlloc_opaque, pState);
    pZip->m_zip_mode = MZ_ZIP_MODE_INVALID;
    return status;
}

#ifndef MINIZ_NO_STDIO
mz_bool mz_zip_add_mem_to_archive_file_in_place(const char *pZip_filename, const char *pArchive_name, const void *pBuf, size_t buf_size, const void *pComment, mz_uint16 comment_size, mz_uint level_and_flags)
{
    mz_bool status, created_new_archive = MZ_FALSE;
    mz_zip_archive zip_archive;
    struct MZ_FILE_STAT_STRUCT file_stat;
    MZ_CLEAR_OBJ(zip_archive);
    if ((int)level_and_flags < 0)
        level_and_flags = MZ_DEFAULT_LEVEL;
    if ((!pZip_filename) || (!pArchive_name) || ((buf_size) && (!pBuf)) || ((comment_size) && (!pComment)) || ((level_and_flags & 0xF) > MZ_UBER_COMPRESSION))
        return MZ_FALSE;
    if (!mz_zip_writer_validate_archive_name(pArchive_name))
        return MZ_FALSE;
    if (MZ_FILE_STAT(pZip_filename, &file_stat) != 0)
    {
        // Create a new archive.
        if (!mz_zip_writer_init_file(&zip_archive, pZip_filename, 0))
            return MZ_FALSE;
        created_new_archive = MZ_TRUE;
    }
    else
    {
        // Append to an existing archive.
        if (!mz_zip_reader_init_file(&zip_archive, pZip_filename, level_and_flags | MZ_ZIP_FLAG_DO_NOT_SORT_CENTRAL_DIRECTORY))
            return MZ_FALSE;
        if (!mz_zip_writer_init_from_reader(&zip_archive, pZip_filename))
        {
            mz_zip_reader_end(&zip_archive);
            return MZ_FALSE;
        }
    }
    status = mz_zip_writer_add_mem_ex(&zip_archive, pArchive_name, pBuf, buf_size, pComment, comment_size, level_and_flags, 0, 0);
    // Always finalize, even if adding failed for some reason, so we have a valid central directory. (This may not always succeed, but we can try.)
    if (!mz_zip_writer_finalize_archive(&zip_archive))
        status = MZ_FALSE;
    if (!mz_zip_writer_end(&zip_archive))
        status = MZ_FALSE;
    if ((!status) && (created_new_archive))
    {
        // It's a new archive and something went wrong, so just delete it.
        int ignoredStatus = MZ_DELETE_FILE(pZip_filename);
        (void)ignoredStatus;
    }
    return status;
}

void *mz_zip_extract_archive_file_to_heap(const char *pZip_filename, const char *pArchive_name, size_t *pSize, mz_uint flags)
{
    int file_index;
    mz_zip_archive zip_archive;
    void *p = NULL;

    if (pSize)
        *pSize = 0;

    if ((!pZip_filename) || (!pArchive_name))
        return NULL;

    MZ_CLEAR_OBJ(zip_archive);
    if (!mz_zip_reader_init_file(&zip_archive, pZip_filename, flags | MZ_ZIP_FLAG_DO_NOT_SORT_CENTRAL_DIRECTORY))
        return NULL;

    if ((file_index = mz_zip_reader_locate_file(&zip_archive, pArchive_name, NULL, flags)) >= 0)
        p = mz_zip_reader_extract_to_heap(&zip_archive, file_index, pSize, flags);

    mz_zip_reader_end(&zip_archive);
    return p;
}

#endif // #ifndef MINIZ_NO_STDIO

#endif // #ifndef MINIZ_NO_ARCHIVE_WRITING_APIS
