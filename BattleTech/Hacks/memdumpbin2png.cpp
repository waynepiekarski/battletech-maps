#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <ctype.h>
#include <math.h>
#include <sys/stat.h>

#include "lodepng/lodepng.cpp"

bool DEBUG = false;

// dosbox-debug has a MEMDUMPBIN [s]:[o] [len] command, this will output a binary dump of the system meory.
// This tool will take this output, tile it into 64K boxes where each tile is 320x200-ish but to 65536 bytes.
// We use 320 because this is the default framebuffer size 320 pixels per row, so hopefully it will reveal any
// data hidden in the memory in a way that is recognizable. We will also allow other tile sizes like 64x64 and
// other sizes which could be useful for data in other formats.

int main (int argc, char* argv[]) {
  if (argc != 3) {
    fprintf(stderr, "Need arguments <memdumpbin.bin> <output_png>\n");
    fprintf(stderr, "Use dosbox-debug with Fn+Alt+P and then this with hex values:\n");
    fprintf(stderr, "MEMDUMPBIN 0:0 100000   <-- Full 1mb including framebuffer\n");
    fprintf(stderr, "MEMDUMPBIN 0:0  A0000   <-- Just memory, no framebuffer\n");
    exit(1);
  }
  FILE *fp = fopen(argv[1], "r");
  if (fp == nullptr) { fprintf(stderr, "File not found"); exit(1); }

  struct stat st;
  stat(argv[1], &st);
  if ((st.st_size != 0x100000) && (st.st_size != 0xA0000)) {
    fprintf(stderr, "%s is not %d or %d as expected\n", argv[1], 0x100000, 0xA0000);
    exit(1);
  }

  // Read the dump into memory
  char memory[1024*1024];
  memset(memory, 0xAA, sizeof(memory));
  int bytes = fread(&memory, 1, st.st_size, fp);
  if (bytes != st.st_size) {
    fprintf(stderr, "Failed to read expected %ld bytes, read only %d\n", st.st_size, bytes);
    exit(1);
  }

  // There are 16 segments of 64k each, so lets create 4x4 tiles
  int tile_width = 320;
  int tile_height = (65536/320);
  if (65536%320 != 0) tile_height++;
  int dtiles = tile_width * tile_height;
  if(DEBUG) fprintf(stderr, "Tile size is %dx%d, dtiles=%d\n", tile_width, tile_height, dtiles);
  assert(tile_width*tile_height >= 65536);
  unsigned char image[4*tile_height][4*tile_width] = { 0x00 };
  int tilecols = 4;
  int tilerows = 4;
  
  for (size_t count = 0; count < 1024*1024; count++) {
    int tilenum = count / 65536;
    int tilecol = tilenum % tilecols;
    int tilerow = tilenum / tilecols;
    int tileofs = count % 65536;
    int tilex = tileofs % tile_width;
    int tiley = tileofs / tile_width;
    int x = tilecol*tile_width + tilex;
    int y = tilerow*tile_height + tiley;
    if(DEBUG) fprintf(stderr, "ofs=%4zu buf=0x%.2x --> tn=%2d,tc=%2d,tr=%2d,to=%2d,tx=%2d,ty=%2d,x=%2d,y=%2d\n", count, memory[count], tilenum, tilecol, tilerow, tileofs, tilex, tiley, x, y);
    assert(tilecol >= 0);
    assert(tilerow >= 0);
    assert(tilecol < tilecols);
    assert(tilerow < tilerows);
    assert(x >= 0);
    assert(y >= 0);
    assert(x < 4*tile_width);
    assert(y < 4*tile_height);
    image[y][x] = memory[count];
  }

#define BTECH_PALETTE
#define VGA_PALETTE

#ifdef VGA_PALETTE
  #include "vga_palette.h"
#endif // VGA_PALETTE
  lodepng::State state;
  srandom(1000); // See the random number to be consistent each time, generates a nice color scheme with this value
  for(int i = 0; i < 256; i++) {
#ifdef VGA_PALETTE
    int j = i*4;
#ifdef BTECH_PALETTE
    // BattleTech has a weird palette scheme where they use either the lower or higher 4-bits for the same 16 colors,
    // this is probably a relic from how it also supports planar output in EGA/VGA.
    if (((i & 0xF0) != 0) && ((i & 0x0F) == 0)) {
      j = ((i & 0xF0) >> 4) * 4;
    }
#endif // BTECH_PALETTE
    unsigned char r = vga_palette[j+0];
    unsigned char g = vga_palette[j+1];
    unsigned char b = vga_palette[j+2];
    unsigned char a = vga_palette[j+3];
#ifdef BTECH_PALETTE
    if ((r == 0xAA) && (g == 0x00) && (b == 0xAA)) {
      r = 0x41;
      g = 0x79;
      b = 0xFF;
    }
#endif // BTECH_PALETTE
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
  unsigned error = lodepng::encode(buffer, &image[0][0], 4*tile_width, 4*tile_height, state);
  if(error) {
    fprintf(stderr, "PNG encoder error %d: %s\n", error, lodepng_error_text(error));
    exit(1);
  }
  lodepng::save_file(buffer, argv[2]);
}
