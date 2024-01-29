#include <cstdio>
#include <cstdlib>
#include <ctype.h>
#include <cstdint>
#include <vector>

int main (int argc, char* argv[]) {
  if (argc != 2) { fprintf(stderr, "No file provided\n"); exit(1); }
  FILE *fp = fopen(argv[1], "r+b");
  if (fp == nullptr) { fprintf(stderr, "File not found\n"); exit(1); }
  printf("GAME filename=%s\n", argv[1]);

  // This doesn't seem to fully work yet, changing just these values over a
  // save game sometimes teleports you somewhere else.

  //std::vector<uint8_t> xxyy = { 0x00, 0x00, 0x00, 0x00 };
  //std::vector<uint8_t> xxyy = { 0xFF, 0xFF, 0xFF, 0xFF };
  // std::vector<uint8_t> xxyy = { 0x0C, 0x1B, 0xC0, 0x00 }; // Default Citadel start
  std::vector<uint8_t> xxyy = { 0x0B, 0x7F, 0xB0, 0x10 }; // Top-left corder outside Citadel

  // X coordinate is at 3910d, Y coordinate is at 3912d, probably 16-bit value?
  fseek(fp, 3910, SEEK_SET);
  for (int c = 0; c < 4; c++) {
    uint8_t b;
    fread(&b, 1, 1, fp);
    fprintf(stderr, "Read byte %d = %.2x\n", 3910+c, b);
  }
  
  fseek(fp, 3910, SEEK_SET);
  int c = 0;
  for (uint8_t each: xxyy) {
    fprintf (stderr, "Write byte %d = %.2x\n", 3910+c, each);
    fputc(each, fp);
    c++;
  }
  if (fclose(fp) != 0) { fprintf (stderr, "Failed to fclose\n"); exit(1); }
  exit(0);
}
