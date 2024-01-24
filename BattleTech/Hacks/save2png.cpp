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
  while ((dir = readdir(d)) != NULL) {
    if (strstr(dir->d_name, "save-y") == NULL)
      continue;

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
    char path [4096];
    sprintf(path, "%s/%s", argv[1], dir->d_name);
    printf("Input path %s with X=[%s]=0x%.3X Y=[%s]=0x%.3X\n", path, xbuf, x, ybuf, y);

    // Decode the PNG
    unsigned char*rgb24 = nullptr;
    unsigned w;
    unsigned h;
    if (lodepng_decode24_file(&rgb24, &w, &h, path) != 0) {
      fprintf (stderr, "Could not decode %s\n", path);
      exit(1);
    }

    // Tiles are 16x16 and screenshots are 216x200
    if ((w != 216) || (h != 200)) {
      fprintf (stderr, "Invalid dimensions %dx%d from %s\n", w, h, path);
    }

  }
  closedir(d);
  return 0;
}
