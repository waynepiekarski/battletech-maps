#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <ctype.h>
#include <math.h>
#include <sys/stat.h>

#include "lodepng/lodepng.cpp"

bool DEBUG = false;

int main (int argc, char* argv[]) {
  if (argc != 3) { fprintf(stderr, "Need arguments <input_mtp> <output_mtp>\n"); exit(1); }
  FILE *fpin = fopen(argv[1], "r");
  if (fpin == nullptr) { fprintf(stderr, "Input file not found"); exit(1); }
  FILE *fpout = fopen(argv[2], "w");
  if (fpout == nullptr) { fprintf(stderr, "Output file not found"); exit(1); }

  struct stat st;
  stat(argv[1], &st);

  int dim;
  int dtiles;
  switch(st.st_size) {
    // Handle 4637 byte MTP files which are actually 64*64=4096 byte images arranged as 8x8 tiles, 8 tiles by 8 tiles
    case 4637:
      dim = 64;
      dtiles = 8;
      break;
    // Handle 1565 byte MTP files which are 32*32=1024 byte images arranged as 8x8 tiles, 4 tiles by 4 tiles
    case 1565:
      dim = 32;
      dtiles = 4;
      break;
    default:
      fprintf(stderr, "Unknown file size %zu\n", st.st_size); exit(1);
      break;
  }

  // Skip header junk
  int count = st.st_size-(dim*dim);
  fprintf(stderr, "Skipping %d header bytes with dim*dim=%d with total size %zu\n", count, dim*dim, st.st_size);
  for (int i = 0; i < count; i++) {
    unsigned char buf;
    int bytes = fread(&buf, 1, 1, fpin);
    if (bytes != 1) {
      if (count == dim*dim) {
        if(DEBUG) fprintf(stderr, "Finished loop due to EOF at count=%d\n", count);
        break;
      } else {
        fprintf(stderr, "Reached EOF at %d before expected %d\n", count, dim*dim);
        exit(1);
      }
    }
    bytes = fwrite(&buf, 1, 1, fpout);
    if (bytes != 1) {
      fprintf(stderr, "Failed to write to output\n");
      exit(1);
    }
  }

  // Remaining bytes to write is dim*dim
  for (int i = 0; i < dim*dim; i++) {
    char ch = i % 256;
    fwrite(&ch, 1, 1, fpout);
  }
  if (fclose(fpout) != 0) {
    fprintf(stderr, "Failed to close output\n");
    exit(1);
  }
  fprintf(stderr, "Finished writing 0..255 output to %s\n", argv[2]);
}
