#ifndef MY_LIBRARY_H
#define MY_LIBRARY_H

#include <stdint.h>

void kaleidoscope(uint8_t *yuvbuf_in, int width, int height,
                  uint8_t *yuvbuf_out, int n_sectors);

#endif /* MY_LIBRARY_H */
