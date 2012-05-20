// example5.c - Demonstrates how to use miniz.c's low-level tdefl_compress() and tinfl_inflate() API's for simple file to file compression/decompression.
// The low-level API's are the fastest, make no use of dynamic memory allocation, and are the most flexible functions exposed by miniz.c.
// Public domain, April 11 2012, Rich Geldreich, richgel99@gmail.com. See "unlicense" statement at the end of tinfl.c.
// For simplicity, this example is limited to files smaller than 4GB, but this is not a limitation of miniz.c.
#include "miniz.c"

typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint;

#define my_max(a,b) (((a) > (b)) ? (a) : (b))
#define my_min(a,b) (((a) < (b)) ? (a) : (b))

// IN_BUF_SIZE is the size of the file read buffer.
// IN_BUF_SIZE must be >= 1
#define IN_BUF_SIZE (1024*512)
static uint8 s_inbuf[IN_BUF_SIZE];

// COMP_OUT_BUF_SIZE is the size of the output buffer used during compression.
// COMP_OUT_BUF_SIZE must be >= 1 and <= OUT_BUF_SIZE
#define COMP_OUT_BUF_SIZE (1024*512)

// OUT_BUF_SIZE is the size of the output buffer used during decompression.
// OUT_BUF_SIZE must be a power of 2 >= TINFL_LZ_DICT_SIZE (because the low-level decompressor not only writes, but reads from the output buffer as it decompresses)
//#define OUT_BUF_SIZE (TINFL_LZ_DICT_SIZE)
#define OUT_BUF_SIZE (1024*512)
static uint8 s_outbuf[OUT_BUF_SIZE];

int main(int argc, char *argv[])
{
   const char *pMode;
   FILE *pInfile, *pOutfile;
   uint infile_size;
   int level = Z_BEST_COMPRESSION;
   int n = 1;
   const char *pSrc_filename;
   const char *pDst_filename;
   const void *next_in = s_inbuf;
   size_t avail_in = 0;
   void *next_out = s_outbuf;
   size_t avail_out = OUT_BUF_SIZE;
   size_t total_in = 0, total_out = 0;

   assert(COMP_OUT_BUF_SIZE <= OUT_BUF_SIZE);

   printf("miniz.c version: %s\n", MZ_VERSION);

   if (argc < 4)
   {
      printf("File to file compression/decompression using the low-level tinfl/tdefl API's.\n");
      printf("Usage: example5 [options] [mode:c or d] infile outfile\n");
      printf("\nModes:\n");
      printf("c - Compresses file infile to a zlib stream in file outfile\n");
      printf("d - Decompress zlib stream in file infile to file outfile\n");
      printf("\nOptions:\n");
      printf("-l[0-10] - Compression level, higher values are slower, 0 is none.\n");
      return EXIT_FAILURE;
   }

   while ((n < argc) && (argv[n][0] == '-'))
   {
      switch (argv[n][1])
      {
         case 'l':
         {
            level = atoi(&argv[1][2]);
            if ((level < 0) || (level > 10))
            {
               printf("Invalid level!\n");
               return EXIT_FAILURE;
            }
            break;
         }
         default:
         {
            printf("Invalid option: %s\n", argv[n]);
            return EXIT_FAILURE;
         }
      }
      n++;
   }

   if ((argc - n) < 3)
   {
      printf("Must specify mode, input filename, and output filename after options!\n");
      return EXIT_FAILURE;
   }
   else if ((argc - n) > 3)
   {
      printf("Too many filenames!\n");
      return EXIT_FAILURE;
   }

   pMode = argv[n++];
   if (!strchr("cCdD", pMode[0]))
   {
      printf("Invalid mode!\n");
      return EXIT_FAILURE;
   }

   pSrc_filename = argv[n++];
   pDst_filename = argv[n++];

   printf("Mode: %c, Level: %u\nInput File: \"%s\"\nOutput File: \"%s\"\n", pMode[0], level, pSrc_filename, pDst_filename);
   
   // Open input file.
   pInfile = fopen(pSrc_filename, "rb");
   if (!pInfile)
   {
      printf("Failed opening input file!\n");
      return EXIT_FAILURE;
   }

   // Determine input file's size.
   fseek(pInfile, 0, SEEK_END);
   infile_size = ftell(pInfile);
   fseek(pInfile, 0, SEEK_SET);

   // Open output file.
   pOutfile = fopen(pDst_filename, "wb");
   if (!pOutfile)
   {
      printf("Failed opening output file!\n");
      return EXIT_FAILURE;
   }

   printf("Input file size: %u\n", infile_size);
     
   if ((pMode[0] == 'c') || (pMode[0] == 'C'))
   {
      uint infile_remaining = infile_size;
      uint comp_flags = tdefl_create_comp_flags_from_zip_params(level, MZ_DEFAULT_WINDOW_BITS, MZ_DEFAULT_STRATEGY) | TDEFL_WRITE_ZLIB_HEADER;

      // Initialize the low-level compressor.
      tdefl_compressor deflator;
      tdefl_status status = tdefl_init(&deflator, NULL, NULL, comp_flags);
      if (status != TDEFL_STATUS_OKAY)
      {
         printf("tdefl_init() failed!\n");
         return EXIT_FAILURE;
      }

      avail_out = COMP_OUT_BUF_SIZE;
      
      // Compression.
      for ( ; ; )
      {
         size_t in_bytes, out_bytes;

         if (!avail_in)
         {
            // Input buffer is empty, so read more bytes from input file.
            uint n = my_min(IN_BUF_SIZE, infile_remaining);

            if (fread(s_inbuf, 1, n, pInfile) != n)
            {
               printf("Failed reading from input file!\n");
               return EXIT_FAILURE;
            }

            next_in = s_inbuf;
            avail_in = n;
            
            infile_remaining -= n;
            //printf("Input bytes remaining: %u\n", infile_remaining);
         }

         in_bytes = avail_in;
         out_bytes = avail_out;
         // Compress as much of the input as possible (or all of it) to the output buffer.
         status = tdefl_compress(&deflator, next_in, &in_bytes, next_out, &out_bytes, infile_remaining ? TDEFL_NO_FLUSH : TDEFL_FINISH);
         
         next_in = (const char *)next_in + in_bytes;
         avail_in -= in_bytes;
         total_in += in_bytes;

         next_out = (char *)next_out + out_bytes;
         avail_out -= out_bytes;
         total_out += out_bytes;

         if ((status != TDEFL_STATUS_OKAY) || (!avail_out))
         {
            // Output buffer is full, or compression is done or failed, so write buffer to output file.
            uint n = COMP_OUT_BUF_SIZE - (uint)avail_out;
            if (fwrite(s_outbuf, 1, n, pOutfile) != n)
            {
               printf("Failed writing to output file!\n");
               return EXIT_FAILURE;
            }
            next_out = s_outbuf;
            avail_out = COMP_OUT_BUF_SIZE;
         }

         if (status == TDEFL_STATUS_DONE)
         {
            // Compression completed successfully.
            break;
         }
         else if (status != TDEFL_STATUS_OKAY)
         {
            // Compression somehow failed.
            printf("tdefl_compress() failed with status %i!\n", status);
            return EXIT_FAILURE;
         }
      }
   }
   else if ((pMode[0] == 'd') || (pMode[0] == 'D'))
   {
      // Decompression.
      uint infile_remaining = infile_size;

      tinfl_decompressor inflator;
      tinfl_init(&inflator);
            
      for ( ; ; )
      {
         size_t in_bytes, out_bytes;
         tinfl_status status;
         if (!avail_in)
         {
            // Input buffer is empty, so read more bytes from input file.
            uint n = my_min(IN_BUF_SIZE, infile_remaining);

            if (fread(s_inbuf, 1, n, pInfile) != n)
            {
               printf("Failed reading from input file!\n");
               return EXIT_FAILURE;
            }

            next_in = s_inbuf;
            avail_in = n;

            infile_remaining -= n;
         }

         in_bytes = avail_in;
         out_bytes = avail_out;
         status = tinfl_decompress(&inflator, (const mz_uint8 *)next_in, &in_bytes, s_outbuf, (mz_uint8 *)next_out, &out_bytes, (infile_remaining ? TINFL_FLAG_HAS_MORE_INPUT : 0) | TINFL_FLAG_PARSE_ZLIB_HEADER);
         
         avail_in -= in_bytes;
         next_in = (const mz_uint8 *)next_in + in_bytes;
         total_in += in_bytes;

         avail_out -= out_bytes;
         next_out = (mz_uint8 *)next_out + out_bytes;
         total_out += out_bytes;
         
         if ((status <= TINFL_STATUS_DONE) || (!avail_out))
         {
            // Output buffer is full, or decompression is done, so write buffer to output file.
            uint n = OUT_BUF_SIZE - (uint)avail_out;
            if (fwrite(s_outbuf, 1, n, pOutfile) != n)
            {
               printf("Failed writing to output file!\n");
               return EXIT_FAILURE;
            }
            next_out = s_outbuf;
            avail_out = OUT_BUF_SIZE;
         }

         // If status is <= TINFL_STATUS_DONE then either decompression is done or something went wrong.
         if (status <= TINFL_STATUS_DONE)
         {
            if (status == TINFL_STATUS_DONE)
            {
               // Decompression completed successfully.
               break;
            }
            else
            {
               // Decompression failed.
               printf("tinfl_decompress() failed with status %i!\n", status);
               return EXIT_FAILURE;
            }
         }
      }
   }
   else
   {
      printf("Invalid mode!\n");
      return EXIT_FAILURE;
   }

   fclose(pInfile);
   if (EOF == fclose(pOutfile))
   {
      printf("Failed writing to output file!\n");
      return EXIT_FAILURE;
   }

   printf("Total input bytes: %u\n", total_in);
   printf("Total output bytes: %u\n", total_out);
   printf("Success.\n");
   return EXIT_SUCCESS;
}
