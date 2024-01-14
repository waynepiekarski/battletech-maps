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
      fprintf(stderr, "Unknown file size %zu\n", (size_t)st.st_size); exit(1);
      break;
  }

  unsigned char image[dim][dim];
  memset(image, 0x00, dim*dim);

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
      image[y][x] = buf;
    }
    count++;
  }

  // Load in all the tile PNG images and store them
  fprintf(stderr, "Loading 256 tiles\n");
  typedef unsigned char tiledef[16][16][3];
  std::vector<tiledef*> tiles;
  for(int i = 0; i < 256; i++) {
    char filename[4096];
    sprintf(filename, "../tile-capture-raw/crop/tile-%03d.png", i);
    // fprintf(stderr, "Loading %s\n", filename);
    unsigned char*rgb24 = nullptr;
    unsigned w;
    unsigned h;
    if (lodepng_decode24_file(&rgb24, &w, &h, filename) != 0) {
      fprintf (stderr, "Could not decode %s\n", filename);
      exit(1);
    }
    if ((w != 16) || (h != 16)) {
      fprintf (stderr, "Tile %dx%d is not 16x16 as expected\n", w, h);
      exit(1);
    }
    tiledef* t = (tiledef*)rgb24;
    tiles.push_back(t);
  }

  // Convert the 8-bit image[y][x] into a full image using the 16x16 tiles
  unsigned w = dim * 16;
  unsigned h = dim * 16;
  unsigned char rgb [h][w][3];
  memset(rgb, 0x00, sizeof(rgb));
  for (int y = 0; y < dim; y++) {
    for (int x = 0; x < dim; x++) {
      unsigned char v = image[y][x];
      tiledef* tile = tiles[v];
      int xofs = x * 16;
      int yofs = y * 16;
      for (int i = 0; i < 16; i++) {
	for (int j = 0; j < 16; j++) {
	  for (int k = 0; k < 3; k++) {
	    rgb[j+yofs][i+xofs][k] = (*tile)[j][i][k];
	  }
	}
      }
    }
  }

  fprintf (stderr, "Writing to %s\n", argv[2]);
  if (lodepng_encode24_file(argv[2], (unsigned char*)rgb, w, h) != 0) {
    fprintf (stderr, "Failed to write to %s\n", argv[2]);
    exit(1);
  }
  fprintf (stderr, "Finished %s\n", argv[2]);
  return 0;
}
