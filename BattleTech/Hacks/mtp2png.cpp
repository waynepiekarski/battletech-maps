#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <ctype.h>
#include <math.h>
#include <sys/stat.h>

#include "lodepng/lodepng.cpp"

bool DEBUG = false;

int main (int argc, char* argv[]) {
  if (argc != 3) { fprintf(stderr, "Need arguments <input_mtp> <output_png>\n"); exit(1); }
  FILE *fp = fopen(argv[1], "r");
  if (fp == nullptr) { fprintf(stderr, "File not found"); exit(1); }

  struct stat st;
  stat(argv[1], &st);
  // size = st.st_size;

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

  unsigned char image[dim][dim] = { 0x00 };

  // Skip header junk
  int count = (dim*dim)-st.st_size;
  while(1) {
    unsigned char buf;
    int bytes = fread(&buf, 1, 1, fp);
    if (bytes != 1) {
      if (count == dim*dim) {
        if(DEBUG) fprintf(stderr, "Finished loop due to EOF at count=%d\n", count);
        break;
      } else {
        fprintf(stderr, "Reached EOF at %d before expected %d\n", count, dim*dim);
        exit(1);
      }
    }

    if (count < 0) {
      // Ignore header junk
    } else {
      int tilenum = count / 64;
      int tilecol = tilenum % dtiles;
      int tilerow = tilenum / dtiles;
      int tileofs = count % 64;
      int tilex = tileofs % 8;
      int tiley = tileofs / 8;
      int x = tilecol*8 + tilex;
      int y = tilerow*8 + tiley;
      if(DEBUG) fprintf(stderr, "ofs=%4d buf=0x%.2x --> tn=%2d,tc=%2d,tr=%2d,to=%2d,tx=%2d,ty=%2d,x=%2d,y=%2d\n", count, buf, tilenum, tilecol, tilerow, tileofs, tilex, tiley, x, y);
      assert(tilecol >= 0);
      assert(tilerow >= 0);
      assert(tilecol < dtiles);
      assert(tilerow < dtiles);
      assert(x >= 0);
      assert(y >= 0);
      assert(x < dim);
      assert(y < dim);
      image[x][y] = buf;
    }
    count++;
  }

// The VGA palette doesn't work well here, use the random palette for now
#undef VGA_PALETTE
#ifdef VGA_PALETTE
  #include "vga_palette.h"
#endif // VGA_PALETTE
  lodepng::State state;
  srandom(1000); // See the random number to be consistent each time, generates a nice color scheme with this value
  for(int i = 0; i < 256; i++) {
#ifdef VGA_PALETTE
    unsigned char r = vga_palette[i*4+0];
    unsigned char g = vga_palette[i*4+1];
    unsigned char b = vga_palette[i*4+2];
    unsigned char a = vga_palette[i*4+3];
#else
    unsigned char r = random() % 256;
    unsigned char g = random() % 256;
    unsigned char b = random() % 256;
    unsigned char a = 255;
#endif // VGA_PALETTE
    lodepng_palette_add(&state.info_png.color, r, g, b, a);
    lodepng_palette_add(&state.info_raw, r, g, b, a);
  }
  state.info_png.color.colortype = LCT_PALETTE;
  state.info_png.color.bitdepth = 8;
  state.info_raw.colortype = LCT_PALETTE;
  state.info_raw.bitdepth = 8;
  state.encoder.auto_convert = 0;

  std::vector<unsigned char> buffer;
  unsigned error = lodepng::encode(buffer, &image[0][0], dim, dim, state);
  if(error) {
    fprintf(stderr, "PNG encoder error %d: %s\n", error, lodepng_error_text(error));
    exit(1);
  }
  lodepng::save_file(buffer, argv[2]);
}
