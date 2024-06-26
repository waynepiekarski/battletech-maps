#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <ctype.h>
#include <math.h>
#include <sys/stat.h>
#include <dirent.h>
#include <set>

#include "lodepng/lodepng.cpp"

bool DEBUG = false;

int main (int argc, char* argv[]) {
  if (argc != 2) { fprintf(stderr, "Need arguments <input_dir>\n"); exit(1); }
  DIR *d;
  struct dirent *dir;
  d = opendir(argv[1]);
  if (!d) {
    fprintf(stderr, "Could not open input directory %s\n", argv[1]);
    exit(1);
  }

  // Allocate an image big enough to store the entire universe
  // Tiles are 16x16 and screenshots are 216x200
  // Maximum tile coordinates are 0x07FF,0x07FF
  // Tile coordinates are actually half a tile, so tiles are effectively 8x8 over 0x0800,0x0800 = 16384x16384
  size_t universe_stride = 16384LL + 200LL; // Add extra padding since 0x7FF extends past the tile
  size_t universe_size = universe_stride * universe_stride;
  printf("Creating universe image with %zu bytes\n", universe_size);
  unsigned char* universe = (unsigned char*)malloc(universe_size);
  // 40d is red in the BattleTech palette, so mark unused map squares to make them obvious in the output
#define EMPTY_PIXEL 40
  memset(universe, EMPTY_PIXEL, universe_size);

  // Save the palette from the last decoded PNG so we can use that for the output encoder
  unsigned char palette[1024];

  size_t incount = 0;
  while ((dir = readdir(d)) != NULL) {
    if (strstr(dir->d_name, "save-y") == NULL)
      continue;
    // if (incount == 10) break; // Debugging to speed up testing

    // Parse the tile coordinates of this image save-y0000-x0000.png
    if (strlen(dir->d_name) != strlen("save-y0000-x0000.png")) {
      fprintf (stderr, "Invalid file name %s\n", dir->d_name);
      exit(1);
    }
    char ybuf[5];
    char xbuf[5];
    strncpy(ybuf, dir->d_name + strlen("save-y"), 4);
    ybuf[4] = '\0';
    strncpy(xbuf, dir->d_name + strlen("save-y0000-x"), 4);
    xbuf[4] = '\0';
    int x = strtol(xbuf, NULL, 16);
    int y = strtol(ybuf, NULL, 16);
    // Compute the actual top-left XY coordinates of this screenshot
    // Note that the tiles are 16x16, but the character moves in a coordinate system where +1 is half a tile
    // So moving +16 like we do is actually equivalent to +8 tiles moving
    int xp = x * 8;
    int yp = y * 8;

    char path [4096];
    sprintf(path, "%s/%s", argv[1], dir->d_name);
    incount++;
    printf("%zu: Processing %s with X=[%s]=0x%.3X Y=[%s]=0x%.3X --> (%d,%d)\n", incount, path, xbuf, x, ybuf, y, xp, yp);

    // Load the PNG into memory so we can use the custom decoder to get the palette
    unsigned char* rawpng = nullptr;
    size_t rawpng_size;
    if (lodepng_load_file(&rawpng, &rawpng_size, path) != 0) {
      fprintf (stderr, "Could not read %s\n", path);
      exit(1);
    }
    
    // Decode the PNG
    unsigned char*tile = nullptr;
    unsigned w;
    unsigned h;
    lodepng::State state;
    lodepng_state_init(&state);
    state.info_png.color.colortype = LCT_PALETTE;
    state.info_png.color.bitdepth = 8;
    state.info_raw.colortype = LCT_PALETTE;
    state.info_raw.bitdepth = 8;
    state.encoder.auto_convert = 0;
    if (lodepng_decode(&tile, &w, &h, &state, rawpng, rawpng_size) != 0) {
      fprintf (stderr, "Could not decode %s\n", path);
      exit(1);
    }

    if ((w != 216) || (h != 200)) {
      fprintf (stderr, "Invalid dimensions %dx%d from %s\n", w, h, path);
      exit(1);
    }

    // Extract out the palette
    if (state.info_png.color.palettesize != 256) {
      fprintf (stderr, "Unknown palettesize %zu\n", state.info_png.color.palettesize);
      exit(1);
    }
    if (incount == 1) {
      printf("Extracting palette for this first image\n");
      memcpy(palette, state.info_png.color.palette, 1024);
      //memcpy(palette, state.info_raw.palette, 1024);
    }

    // Copy the PNG into the universe
    unsigned char *srcptr = tile;
    unsigned char *dstptr = universe + (yp*universe_stride) + xp;
    for (int r = 0; r < 200; r++) {
      // Could use memcpy for this to be faster, but do detailed pixel checking instead to ensure we do the tiling perfectly
      // memcpy(dstptr, srcptr, 216);
      const unsigned char *s = srcptr;
      unsigned char *d = dstptr;
      for (int c = 0; c < 216; c++) {
#undef GLITCH_PROBE
#ifdef GLITCH_PROBE
	// Implement probe where we can check if this image is touching exactly a specific pixel. Can use this to find
	// glitches in the final output map and to work out what inputs contributed to it.
	int dx = xp + c;
	int dy = yp + r;
	if ((dx == 10691) && (dy == 6288)) {
	  printf("Glitch probe with dx=%d,dy=%d for %s\n", dx, dy, path);
	}
#endif // GLITCH_PROBE
	/*
	if (*d != EMPTY_PIXEL) {
	  // Dest has already been set previously, check the image matches as expected
	  // Cannot use this test, because the lightning fence around the Citadel is animated
	  // and varies between frames. Also found a few other random cases in the top-left
	  // with a green blob at (0,0) and just some other unexplained errors.
	  if (*d != *s) {
	    unsigned char rd = palette[(*d)*4+0];
	    unsigned char gd = palette[(*d)*4+1];
	    unsigned char bd = palette[(*d)*4+2];
	    unsigned char rs = palette[(*s)*4+0];
	    unsigned char gs = palette[(*s)*4+1];
	    unsigned char bs = palette[(*s)*4+2];
	    if ((rd == rs) && (gd == gs) && (bd == bs)) {
	      printf("Found pixel index mismatch but same color %s x=%d y=%d dest=%.2X(%.2X%.2X%.2X) src=%.2X(%.2X%.2X%.2X)\n", path, c, r, *d, rd, gd, bd, *s, rs, gs, bs);
	    } else {
	      printf("Found pixel total mismatch %s x=%d y=%d dest=%.2X(%.2X%.2X%.2X) src=%.2X(%.2X%.2X%.2X)\n", path, c, r, *d, rd, gd, bd, *s, rs, gs, bs);
	    }
	    // exit(1);
	  }
	}
	*/
	if (d >= universe+universe_size) {
	  fprintf(stderr, "Exceeding universe buffer at r=%d,c=%d\n", r, c);
	  exit(1);
	}
	*d = *s;
	s++;
	d++;
      }
      srcptr += 216;
      dstptr += universe_stride;
    }
  }
  closedir(d);

  std::set<int> empty;
  printf("Finding empty pixel coordinates to revisit\n");
  size_t empty_pixels = 0;
  for (int y = 0; y < 16384; y++) {
    for (int x = 0; x < 16384; x++) {
      if (universe[y*universe_stride+x] == EMPTY_PIXEL) {
	// Only emit one per game location, but also we move in steps of 16 so
	// we can chop off the bottom 4 bits as well
	int hash = ((x/8)>>4) + (((y/8) >> 4) << 12);
	auto r = empty.insert(hash);
	if (r.second == true) {
	  printf("BTECHGO 0x%.4X 0x%.4X\n", x/8, y/8);
	}
	empty_pixels++;
      }
    }
  }
  printf("Found %zu empty pixels\n", empty_pixels);
  
  // Write out the universe
  // ImageMagick can't even run "identify" on an image wider than 16000 pixels, so cannot handle 65536 pixels
  unsigned char* galaxy = (unsigned char*)malloc(4096*4096);
  for (int ty = 0; ty < 4; ty++) {
    for (int tx = 0; tx < 4; tx++) {
      // Copy the universe into a separate 4096x4096 image so we can save it separately
      char outfile [4096];
      sprintf(outfile, "universe-y%02d-x%02d.png", ty, tx); // Y-row ordering for icon previews in file explorer
      printf("Encoding 4096x4096 %d,%d universe tile image to %s\n", tx, ty, outfile);
      const unsigned char *srcptr = universe + (ty*4096*universe_stride) + tx*4096;
      unsigned char *dstptr = galaxy;
      for (int r = 0; r < 4096; r++) {
	memcpy(dstptr, srcptr, 4096);
	srcptr += universe_stride;
	dstptr += 4096;
      }

      lodepng::State stateout;
      lodepng_state_init(&stateout);
      srandom(1000);
      for(int i = 0; i < 256; i++) {
	unsigned char r = palette[i*4+0];
	unsigned char g = palette[i*4+1];
	unsigned char b = palette[i*4+2];
	unsigned char a = 255;
	// RGBA values must be the same for both input and output palettes, otherwise it fails to convert inside lodepng
	lodepng_palette_add(&stateout.info_png.color, r, g, b, a);
	lodepng_palette_add(&stateout.info_raw, r, g, b, a);
      }
      stateout.info_png.color.colortype = LCT_PALETTE;
      stateout.info_png.color.bitdepth = 8;
      stateout.info_raw.colortype = LCT_PALETTE;
      stateout.info_raw.bitdepth = 8;
      stateout.encoder.auto_convert = 0;

      std::vector<unsigned char> outbuf;
      unsigned result = lodepng::encode(outbuf, galaxy, 4096, 4096, stateout);
      if (result != 0) {
	fprintf(stderr, "Failed to encode %s palette image - error %d\n", outfile, result);
	exit(1);
      }
      printf("Writing %s to disk\n", outfile);
      if (lodepng::save_file(outbuf, outfile) != 0) {
	fprintf(stderr, "Could not write out %s\n", outfile);
	exit(1);
      }
    }
  }
  free(galaxy);

  // Write out one big mega image, but we can't write universe out directly since it has padding around the edges, so need to make a copy
  {
    unsigned char* galaxy = (unsigned char*)malloc(16384*16384);
    const char *outfile = "universe-all.png";
    printf("Saving full universe-all.png image\n");
    const unsigned char *srcptr = universe;
    unsigned char *dstptr = galaxy;
    for (int r = 0; r < 16384; r++) {
      memcpy(dstptr, srcptr, 16384);
      srcptr += universe_stride;
      dstptr += 16384;
    }
    
    lodepng::State stateout;
    lodepng_state_init(&stateout);
    srandom(1000);
    for(int i = 0; i < 256; i++) {
      unsigned char r = palette[i*4+0];
      unsigned char g = palette[i*4+1];
      unsigned char b = palette[i*4+2];
      unsigned char a = 255;
      // RGBA values must be the same for both input and output palettes, otherwise it fails to convert inside lodepng
      lodepng_palette_add(&stateout.info_png.color, r, g, b, a);
      lodepng_palette_add(&stateout.info_raw, r, g, b, a);
    }
    stateout.info_png.color.colortype = LCT_PALETTE;
    stateout.info_png.color.bitdepth = 8;
    stateout.info_raw.colortype = LCT_PALETTE;
    stateout.info_raw.bitdepth = 8;
    stateout.encoder.auto_convert = 0;
    
    std::vector<unsigned char> outbuf;
    unsigned result = lodepng::encode(outbuf, galaxy, 16384, 16384, stateout);
    if (result != 0) {
      fprintf(stderr, "Failed to encode %s palette image - error %d\n", outfile, result);
      exit(1);
    }
    printf("Writing %s to disk\n", outfile);
    if (lodepng::save_file(outbuf, outfile) != 0) {
      fprintf(stderr, "Could not write out %s\n", outfile);
      exit(1);
    }
    free(galaxy);
  }

  return 0;
}
