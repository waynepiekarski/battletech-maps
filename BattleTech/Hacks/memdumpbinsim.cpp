#include <string.h>
#include <stdio.h>
#include <stdlib.h>

int main (int argc, char* argv[]) {
  if ((argc == 1) || (!strcmp(argv[1],"16"))) {
    // Generate 64k segments of data, use a unique value for each segment so we can test the PNG converter
    for (unsigned char value = 0; value < 16; value++) {
      for (size_t bytes = 0; bytes < 65536; bytes++) {
	fputc(value, stdout);
      }
    }
  } else if (!strcmp(argv[1],"256")) {
    for (unsigned char tile = 0; tile < 16; tile++) {
      for (size_t bytes = 0; bytes < 65536; bytes++) {
	// Each tile needs to render 16 colors from the palette, so 65536/16=4096 pixels per color
	// Each tile is 320x pixels across, so need to align with those. 4096/320=12.8 rows
	unsigned char color = bytes / (320*(12+1));
	unsigned char value = (unsigned char)color + (tile*16);
	if (color >= 16) { value = 0; }
	fputc(value, stdout);
      }
    }
  } else if (!strcmp(argv[1],"0")) {
    for (unsigned char tile = 0; tile < 16; tile++) {
      for (size_t bytes = 0; bytes < 65536; bytes++) {
	fputc(0x00, stdout);
      }
    }
  } else {
    fprintf(stderr, "Unknown argv[1]=%s\n", argv[1]);
    exit(1);
  }
}
