#include <cstdio>
#include <cstdlib>
#include <ctype.h>

int main (int argc, char* argv[]) {
  if (argc != 3) { fprintf(stderr, "No file provided"); exit(1); }
  FILE *fp = fopen(argv[1], "r");
  if (fp == nullptr) { fprintf(stderr, "File not found"); exit(1); }
  int width = atoi(argv[2]);
  printf("FILENAME=%s WIDTH=%d\n", argv[1], width);

  unsigned int count = 0;
  while(1) {
    unsigned char buf;
    int bytes = fread(&buf, 1, 1, fp);
    if (bytes != 1) {
      exit(0);
    }

    // 1-byte text-only dump format
    if (isprint(buf)) { printf("%c", buf); } else { printf("%c", '.'); }
    // 2-byte hex dump format
    // printf("%.02x", buf);
    count++;
    if (count == width) { printf("\n"); count = 0; }
  }
}
