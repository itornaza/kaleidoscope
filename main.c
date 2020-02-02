#include "my_library.h"
#include "utils.h"

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define INPLACE 0

int main(int argc, char *argv[]) {
  const char *inpath, *outpath;
  uint8_t *yuvbuf_in = NULL;
  uint8_t *yuvbuf_out;
  int width, height;
  int n_sectors = 6; // Number of sectors of the kaleidoscope

  if (argc != 3) {
    fprintf(stderr, "Usage: %s <jpeg in> <jpeg out>\n", argv[0]);
    return 1;
  }

  inpath = argv[1];
  outpath = argv[2];

  read_jpeg_to_i420(inpath, &width, &height, &yuvbuf_in);
  assert(yuvbuf_in);

  if (INPLACE) {
    yuvbuf_out = yuvbuf_in;
  } else {
    yuvbuf_out = malloc(width * height * 3 / 2);
    assert(yuvbuf_out);
  }

  kaleidoscope(yuvbuf_in, width, height, yuvbuf_out, n_sectors);

  write_i420_to_jpeg(yuvbuf_out, width, height, outpath);

  free(yuvbuf_in);

  if (!INPLACE)
    free(yuvbuf_out);

  return 0;
}
