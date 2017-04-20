#include "my_library.h"

#include <string.h>

void
my_function (uint8_t * yuvbuf_in, int width, int height, uint8_t * yuvbuf_out)
{
  /* Do something here */
  memmove (yuvbuf_out, yuvbuf_in, width * height * 3 / 2);
}
