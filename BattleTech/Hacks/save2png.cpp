#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <ctype.h>
#include <math.h>
#include <sys/stat.h>
#include <dirent.h>

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
  // Maximum coordinates are 0x0FFF,0x0FFF = 4096,4096
  // In reality, X is limited to 0xF7F since this is the top-left corner
  size_t universe_size = 4096LL * 16LL * 4096LL * 16LL;
  size_t universe_stride = 4096*16;
  fprintf(stderr, "Creating universe image with %zu bytes\n", universe_size);
  unsigned char* universe = (unsigned char*)malloc(universe_size);
  memset(universe, 0xAA, universe_size);

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
    int xp = x * 16;
    int yp = y * 16;

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
    }

    // Extract out the palette
    if (state.info_png.color.palettesize != 256) {
      fprintf (stderr, "Unknown palettesize %zu\n", state.info_png.color.palettesize);
      exit(1);
    }
    if (incount == 1) {
      fprintf(stderr, "Extracting palette for this first image\n");
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
	if (*d != 0xAA) {
	  // Dest has already been set previously, check the image matches as expected
	  if (*d != *s) {
	    // TODO: Currently this code fails, need to look into this
	    //fprintf(stderr, "Found pixel mismatch dest=%.2X src=%.2X\n", *d, *s);
	    //exit(1);
	  }
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

  /*
  fprintf(stderr, "Counting empty pixels\n");
  size_t empty_pixels = 0;
  for (size_t i = 0; i < universe_size; i++) {
    if (universe[i] == 0xAA) {
      empty_pixels++;
    }
  }
  fprintf(stderr, "Found %zu empty pixels\n", empty_pixels);
  */
  
  // Write out the universe
  // ImageMagick can't even run "identify" on an image wider than 16000 pixels, so cannot handle 65536 pixels
  unsigned char* galaxy = (unsigned char*)malloc(4096*4096);
  for (int ty = 0; ty < /*16*/1; ty++) {
    for (int tx = 0; tx < /*16*/1; tx++) {
      // Copy the universe into a separate 4096x4096 image so we can save it separately
      char outfile [4096];
      sprintf(outfile, "universe-%02d-%02d.png", tx, ty);
      fprintf(stderr, "Encoding 4096x4096 %d,%d universe tile image to %s\n", tx, ty, outfile);
      const unsigned char *srcptr = universe + (ty*4096*universe_stride) + tx*4096;
      unsigned char *dstptr = galaxy;
      for (int r = 0; r < 4096; r++) {
	memcpy(dstptr, srcptr, 4096);
	//memset(dstptr, 0x01, 4096);
	/*for (int junk = 0; junk < 4096; junk++) {
	  *(dstptr + junk) = rand() % 255;
	  } */
	srcptr += universe_stride;
	dstptr += 4096;
      }

      // This grayscale save works ok
      fprintf(stderr, "Saving temp.png test image\n");
      if (lodepng_encode_file("temp.png", galaxy, 4096, 4096, LCT_GREY, 8) != 0) {
	fprintf(stderr, "Failed to save temp test image\n");
	exit(1);
      }

      // This encode always writes out a black image
      lodepng::State stateout;
      lodepng_state_init(&stateout);
      srandom(1000);
      for(int i = 0; i < 256; i++) {
	/*
	unsigned char r = random() % 256;
	unsigned char g = random() % 256;
	unsigned char b = random() % 256;
	unsigned char a = 255;
	*/
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
      fprintf(stderr, "Writing %s to disk\n", outfile);
      if (lodepng::save_file(outbuf, outfile) != 0) {
	fprintf(stderr, "Could not write out %s\n", outfile);
	exit(1);
      }
    }
  }

  return 0;
}
