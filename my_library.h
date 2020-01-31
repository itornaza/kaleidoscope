#ifndef MY_LIBRARY_H
#define MY_LIBRARY_H

#include <stdint.h>

void my_crazy_function(uint8_t *yuvbuf_in, int width, int height,
                       uint8_t *yuvbuf_out);

void my_function(uint8_t *yuvbuf_in, int width, int height,
                 uint8_t *yuvbuf_out, int n_sectors);

void clone_pixel(uint8_t *yuvbuf, int width, int height, int w, int h,
                   double fi, int n_sectors, int y, int u, int v, int u_off,
                   int v_off);

void dim_image(uint8_t *yuvbuf, int width, int height, int f_dim);

#endif /* MY_LIBRARY_H */
