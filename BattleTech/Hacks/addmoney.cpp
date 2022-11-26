#include <cstdio>
#include <cstdlib>
#include <ctype.h>

int main (int argc, char* argv[]) {
  if (argc != 2) { fprintf(stderr, "No file provided\n"); exit(1); }
  FILE *fp = fopen(argv[1], "r+b");
  if (fp == nullptr) { fprintf(stderr, "File not found\n"); exit(1); }
  printf("GAME filename=%s\n", argv[1]);

  // 4 bytes per account value, write 0x7F to not become negative
  // C-bills, DefHes, NasDiv, BakPhar (0xD69-0xD6C)
  fseek(fp, 0xD5A, SEEK_SET);
  int count = 0xD6C-0xD5A;
  while(count >= 0) {
    fputc(0x7F, fp);
    count--;
  }
  if (fclose(fp) != 0) { fprintf (stderr, "Failed to fclose\n"); exit(1); }
  exit(0);
}
