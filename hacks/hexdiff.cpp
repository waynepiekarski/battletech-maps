#include <cstdio>
#include <cstdlib>
#include <ctype.h>
#include <vector>
#include <string.h>

// Diff two files but using hex addresses and values in hex as well as how much the delta was
bool flag_no_graphics = false;

int main (int argc, char* _argv[]) {
  std::vector<char*> argv;
  for(int i = 0; i < argc; i++) {
    if (!strcmp(_argv[i], "--no-graphics")) {
      flag_no_graphics = true;
      fprintf(stderr, "Will stop before A000:0000 graphics memory\n");
    } else {
      argv.push_back(_argv[i]);
    }
  }
  if (argv.size() != 3) { fprintf(stderr, "Two files required"); exit(1); }
  FILE *fpa = fopen(argv[1], "r");
  if (fpa == nullptr) { fprintf(stderr, "File %s not found", argv[1]); exit(1); }
  FILE *fpb = fopen(argv[2], "r");
  if (fpb == nullptr) { fprintf(stderr, "File %s not found", argv[2]); exit(1); }
  printf("%s %s %s\n", argv[0], argv[1], argv[2]);

  size_t ofs = 0;
  size_t last = 0;
  while(1) {
    if ((flag_no_graphics) && (ofs >= 0xA0000)) {
      fprintf(stderr, "Stopping before A000:0000 framebuffer\n");
      exit(0);
    }
    unsigned char a;
    int aa = fread(&a, 1, 1, fpa);
    unsigned char b;
    int bb = fread(&b, 1, 1, fpb);
    if ((aa != 1) && (bb != 1)) {
      fprintf(stderr, "Finished diff\n");
      exit(0);
    }
    if (aa != 1) {
      fprintf(stderr, "Early EOF on %s\n", argv[1]);
      exit(1);
    }
    if (bb != 1) {
      fprintf(stderr, "Early EOF on %s\n", argv[2]);
      exit(1);
    }

    if (a == b) {
      ofs++;
      continue;
    }

    printf("ofs=0x%.5zX lastofs=0x%-5zX src=0x%.2X dst=0x%.2X diffdec=%+d\n", ofs, ofs-last, a, b, (int)b-(int)a);
    fflush(stdout);
    last = ofs;
    ofs++;
  }
}
