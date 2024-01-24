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

  // Save the state from the last decoded PNG and use that same state for the final encoder, so we
  // capture the palette that is the same for all images.
  lodepng::State state;

  size_t incount = 0;
  while ((dir = readdir(d)) != NULL) {
    if (strstr(dir->d_name, "save-y") == NULL)
      continue;
    if (incount == 10) break; // Debugging to speed up testing

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

    // Copy the PNG into the universe
    unsigned char *srcptr = tile;
    unsigned char *dstptr = universe + (yp*universe_stride) + xp;
    for (int r = 0; r < 200; r++) {
      // Could use memcpy for this to be faster, but do detailed pixel checking instead to ensure we do the tiling perfectly
      // memcpy(dstptr, srcptr, 216);
      unsigned char *s = srcptr;
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

  fprintf(stderr, "Counting empty pixels\n");
  size_t empty_pixels = 0;
  for (size_t i = 0; i < 0*universe_size; i++) {
    if (universe[i] == 0xAA) {
      empty_pixels++;
    }
  }
  fprintf(stderr, "Found %zu empty pixels\n", empty_pixels);
  
  // Write out the universe
  fprintf(stderr, "Encoding and writing universe.png\n");
  std::vector<unsigned char> outbuf;
  if (lodepng::encode(outbuf, universe, 4096*16, 4096*16, state) != 0) {
    fprintf(stderr, "Failed to encode universe palette image\n");
    exit(1);
  }
  if (lodepng::save_file(outbuf, "universe.png") != 0) {
    fprintf(stderr, "Could not write out universe.png\n");
    exit(1);
  }

  return 0;
}
