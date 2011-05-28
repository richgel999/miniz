// example2.c - Simple demonstration of miniz.c's ZIP archive API's.
// Public domain, May 15 2011, Rich Geldreich, richgel99@gmail.com. See "unlicense" statement at the end of tinfl.c.
#include "miniz.c"

typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint;

// The string to compress.
static const char *s_pStr = 
  "MISSION CONTROL I wouldn't worry too much about the computer. First of all, there is still a chance that he is right, despite your tests, and" \
  "if it should happen again, we suggest eliminating this possibility by allowing the unit to remain in place and seeing whether or not it" \
  "actually fails. If the computer should turn out to be wrong, the situation is still not alarming. The type of obsessional error he may be" \
  "guilty of is not unknown among the latest generation of HAL 9000 computers. It has almost always revolved around a single detail, such as" \
  "the one you have described, and it has never interfered with the integrity or reliability of the computer's performance in other areas." \
  "No one is certain of the cause of this kind of malfunctioning. It may be over-programming, but it could also be any number of reasons. In any" \
  "event, it is somewhat analogous to human neurotic behavior. Does this answer your query?  Zero-five-three-Zero, MC, transmission concluded.";

static const char *s_pComment = "This is a comment";

int main(int argc, char *argv[])
{
  uint i;
  mz_bool status;
  size_t uncomp_size;
  mz_zip_archive zip_archive; 
  void *p;

  printf("miniz.c version: %s\n", MZ_VERSION);

  argc, argv;

  // Append a bunch of text files to test.zip
  for (i = 0; i < 50; i++)
  {
    char archive_filename[64];
    sprintf(archive_filename, "%u.txt", i);
    status = mz_zip_add_mem_to_archive_file_in_place("test.zip", archive_filename, s_pStr, strlen(s_pStr), s_pComment, (uint16)strlen(s_pComment), MZ_BEST_COMPRESSION);
    if (!status)
    {
      printf("mz_zip_add_mem_to_archive_file_in_place failed!\n");
      return EXIT_FAILURE;
    }
  }
  
  // Now try to open the archive.
  memset(&zip_archive, 0, sizeof(zip_archive));
  status = mz_zip_reader_init_file(&zip_archive, "test.zip", 0);
  if (!status)
  {
    printf("mz_zip_reader_init_file() failed!\n");
    return EXIT_FAILURE;
  }

  // Get and print information about each file in the archive.
  for (i = 0; i < mz_zip_reader_get_num_files(&zip_archive); i++)
  {
    mz_zip_archive_file_stat file_stat;
    if (!mz_zip_reader_file_stat(&zip_archive, i, &file_stat))
    {
      printf("mz_zip_reader_file_stat() failed!\n");
      mz_zip_reader_end(&zip_archive);
      return EXIT_FAILURE;
    }

    printf("Filename: \"%s\", Comment: \"%s\", Uncompressed size: %u, Compressed size: %u\n", file_stat.m_filename, file_stat.m_comment, (uint)file_stat.m_uncomp_size, (uint)file_stat.m_comp_size);
  }
  
  // Try to extract 0.txt to the heap.
  p = mz_zip_reader_extract_file_to_heap(&zip_archive, "0.txt", &uncomp_size, 0);
  if (!p)
  {
    printf("mz_zip_reader_extract_file_to_heap() failed!\n");
    mz_zip_reader_end(&zip_archive);
    return EXIT_FAILURE;
  }
    
  // Make sure the extraction really succeeded.
  if ((uncomp_size != strlen(s_pStr)) || (memcmp(p, s_pStr, strlen(s_pStr))))
  {
    printf("mz_zip_reader_extract_file_to_heap() failed to extract the proper data\n");
    free(p);
    mz_zip_reader_end(&zip_archive);
    return EXIT_FAILURE;
  }

  printf("Successfully extracted file \"0.txt\", size %u\n", (uint)uncomp_size);

  // We're done.
  free(p);
  mz_zip_reader_end(&zip_archive);

  printf("Success.\n");
  return EXIT_SUCCESS;
}
