#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <ctype.h>
#include <math.h>

#include "lodepng/lodepng.cpp"

bool DEBUG = false;

// Handle 4637 byte MTP files which are actually 64*64=4096 byte images arranged as 8x8 tiles
int main (int argc, char* argv[]) {
  if (argc != 3) { fprintf(stderr, "Need arguments <input_mtp> <output_png>\n"); exit(1); }
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
      image[x][y] = buf;
    }
    count++;
  }

  lodepng::State state;
  srandom(1000); // See the random number to be consistent each time, generates a nice color scheme with this value
  for(int i = 0; i < 256; i++) {
    unsigned char r = random() % 256;
    unsigned char g = random() % 256;
    unsigned char b = random() % 256;
    unsigned char a = 255;
    lodepng_palette_add(&state.info_png.color, r, g, b, a);
    lodepng_palette_add(&state.info_raw, r, g, b, a);
  }
  state.info_png.color.colortype = LCT_PALETTE;
  state.info_png.color.bitdepth = 8;
  state.info_raw.colortype = LCT_PALETTE;
  state.info_raw.bitdepth = 8;
  state.encoder.auto_convert = 0;

  std::vector<unsigned char> buffer;
  unsigned error = lodepng::encode(buffer, &image[0][0], 64, 64, state);
  if(error) {
    fprintf(stderr, "PNG encoder error %d: %s\n", error, lodepng_error_text(error));
    exit(1);
  }
  lodepng::save_file(buffer, argv[2]);
}
