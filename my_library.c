#include "my_library.h"

#include <math.h>
#include <stdio.h>
#include <string.h>

void my_function(uint8_t *yuvbuf_in, int width, int height,
                 uint8_t *yuvbuf_out) {

  /* Do something here */
  memmove(yuvbuf_out, yuvbuf_in, width * height * 3 / 2);

  double n_sectors = 6.0;           // The numner of sectors of the kaleidoscope
  double theta = 0.0;               // θ is the running angle
  double theta_inc = 0.00001;       // θ increment step
  double fi = 2 * M_PI / n_sectors; // φ is the sector angle
  int r_max = width / 4;            // Kaleidoskope max range from image center
  int r_inc = width / 1000;         // radious increment step

  while (fi - theta >= 0.1) {

    // For radial pixels starting from origin to kaleidoskope outer boundary
    for (int r = 0; r < r_max; r += r_inc) {

      // 1. Convert polar to cartesian coordinates
      int h = r * sin(theta);
      int w = r * cos(theta);

      // 2. Shift origin from the top left corner to the middle of the image
      h += height / 2;
      w += width / 2;

      // 3. Get the YUV values of the pixel
      int u_off = width * height;
      int v_off = u_off + (width / 2) * (height / 2);
      int uv_idx = (h / 2) * (width / 2) + (w / 2);
      int Y = yuvbuf_out[h * width + w];
      int U = yuvbuf_out[u_off + uv_idx];
      int V = yuvbuf_out[v_off + uv_idx];

      // 4. Assign pixel YUV value to the rest of the arcs
      for (int s = 0; s <= n_sectors; ++s) {
        h = r * sin(theta + s * fi + M_PI / 2.0) + height / 2;
        w = r * cos(theta + s * fi + M_PI / 2.0) + width / 2;
        
        // TODO: Adjust the indices

        yuvbuf_out[h * width + w] = Y;
        yuvbuf_out[u_off + uv_idx] = U;
        yuvbuf_out[v_off + uv_idx] = V;
      }
    }
    theta += theta_inc;
  }
}
