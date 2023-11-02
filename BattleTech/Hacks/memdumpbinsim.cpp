#include <stdio.h>

int main (int argc, char* argv[]) {
  // Generate 64k segments of data, use a unique value for each segment so we can test the PNG converter
  for (unsigned char value = 0; value < 16; value++) {
    for (size_t bytes = 0; bytes < 65536; bytes++) {
      fputc(value, stdout); 
    }
  }
}
