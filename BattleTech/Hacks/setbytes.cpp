#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <ctype.h>
#include <math.h>
#include <sys/stat.h>

#include "lodepng/lodepng.cpp"

bool DEBUG = false;

int main (int argc, char* argv[]) {
  if (argc != 5) { fprintf(stderr, "Need arguments <file> <startofshex> <endofshex> <valhex>\n"); exit(1); }
  FILE *fpin = fopen(argv[1], "r+");
  if (fpin == nullptr) { fprintf(stderr, "Input file not found"); exit(1); }
  int startofs = strtol(argv[2], NULL, 16);
  int endofs = strtol(argv[3], NULL, 16);
  int val = strtol(argv[4], NULL, 16);
  fprintf (stderr, "Patching %s from 0x%X..0x%X (%d..%d) with 0x%X\n", argv[1], startofs, endofs, startofs, endofs, val);

  fseek(fpin, startofs, SEEK_SET);
  for (int i = startofs; i <= endofs; i++) {
    fputc(val, fpin);
  }
  fclose(fpin);
  fprintf (stderr, "Finished writing %d bytes\n", endofs-startofs+1);
}
