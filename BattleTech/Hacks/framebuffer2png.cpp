#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <ctype.h>
#include <math.h>
#include <sys/stat.h>

#include "lodepng/lodepng.cpp"

bool DEBUG = false;

// dosbox-debug has a MEMDUMPBIN [s]:[o] [len] command, this will output a binary dump of the system meory.
// This tool will take either 64K or the 64K located at A000:0000 and dump it to a 320x200 image.

int main (int argc, char* argv[]) {
  if (argc != 3) {
    fprintf(stderr, "Need arguments <memdumpbin.bin> <output_png>\n");
    fprintf(stderr, "Must run BattleTech with MCGA for linear mode and not EGA/VGA with planar mode\n");
    fprintf(stderr, "Use dosbox-debug with Fn+Alt+P and then this with hex values:\n");
    fprintf(stderr, "MEMDUMPBIN 0:0 100000\n");
    fprintf(stderr, "MEMDUMPBIN A000:0 10000\n");
    exit(1);
  }
  FILE *fp = fopen(argv[1], "r");
  if (fp == nullptr) { fprintf(stderr, "File not found"); exit(1); }

  struct stat st;
  stat(argv[1], &st);
  size_t ofs = 0;
  if (st.st_size == 1024*1024) {
    ofs = 0xA0000;
  } else if (st.st_size == 64*1024) {
    ofs = 0;
  } else {
    fprintf(stderr, "%s is not 64K or 1M as expected\n", argv[1]);
    exit(1);
  }

  // Read the image which is always 320x200, need to use MCGA mode when running BattleTech and not
  // EGA/VGA which uses a weird 8-bit planar mode that cannot be decoded this way.
  unsigned char memory[320*200];
  if (fseek(fp, ofs, SEEK_SET) != 0) {
    fprintf(stderr, "Seek error\n");
    exit(1);
  }
  int bytes = fread(&memory, 1, 320*200, fp);
  if (bytes != 320*200) {
    fprintf(stderr, "Failed to read 320*200 bytes = %d\n", bytes);
    exit(1);
  }

#define BTECH_PALETTE
#define VGA_PALETTE

#ifdef VGA_PALETTE
  #include "vga_palette.h"
#endif // VGA_PALETTE
  lodepng::State state;
  srandom(1000); // See the random number to be consistent each time, generates a nice color scheme with this value
  for(unsigned int i = 0; i < 256; i++) {
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
  unsigned error = lodepng::encode(buffer, &memory[0], 320, 200, state);
  if(error) {
    fprintf(stderr, "PNG encoder error %d: %s\n", error, lodepng_error_text(error));
    exit(1);
  }
  lodepng::save_file(buffer, argv[2]);
}
