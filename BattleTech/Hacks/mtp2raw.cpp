#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <ctype.h>

bool DEBUG = false;

// Handle 4637 byte MTP files which are actually 64*64=4096 byte images arranged as 8x8 tiles
int main (int argc, char* argv[]) {
  if (argc != 2) { fprintf(stderr, "No file provided"); exit(1); }
  FILE *fp = fopen(argv[1], "r");
  if (fp == nullptr) { fprintf(stderr, "File not found"); exit(1); }

  unsigned char image[64][64] = { 0x00 };

  // Skip header junk
  int count = 4096-4637;
  while(1) {
    unsigned char buf;
    int bytes = fread(&buf, 1, 1, fp);
    if (bytes != 1) {
      if (count == 4096) {
        if(DEBUG) fprintf(stderr, "Finished loop due to EOF at count=%d\n", count);
        break;
      } else {
        fprintf(stderr, "Reached EOF before expected 4096\n");
        exit(1);
      }
    }

    if (count < 0) {
      // Ignore header junk
    } else {
      // Convert byte offset into 8x8=64byte tile coordinates
      int tilenum = count / 64;
      int tilecol = tilenum % 8;
      int tilerow = tilenum / 8;
      int tileofs = count % 64;
      int tilex = tileofs % 8;
      int tiley = tileofs / 8;
      int x = tilecol*8 + tilex;
      int y = tilerow*8 + tiley;
      assert(x >= 0);
      assert(y >= 0);
      assert(x < 64);
      assert(y < 64);
      if(DEBUG) fprintf(stderr, "ofs=%d buf=0x%.2x --> tn=%d,tc=%d,tr=%d,to=%d,tx=%d,ty=%d,x=%d,y=%d\n", count, buf, tilenum, tilecol, tilerow, tileofs, tilex, tiley, x, y);
      image[y][x] = buf;
    }
    count++;
  }

  for (int x = 0; x < 64; x++) {
    for (int y = 0; y < 64; y++) {
      fputc(image[y][x], stdout);
    }
  }
}
