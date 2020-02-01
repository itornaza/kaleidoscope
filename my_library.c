#include "my_library.h"

#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * kaleidoscope
 *
 * Implements the kaleidoscope effect with an adjustable number of sectors
 */
void kaleidoscope(uint8_t *yuvbuf_in, int width, int height,
                  uint8_t *yuvbuf_out, int n_sectors) {
  // Buffers
  int buf_size = width * height * 3 / 2;
  memmove(yuvbuf_out, yuvbuf_in, buf_size);

  // Offsets for indices of the YUV sections within the frame buffers
  int u_off = width * height;
  int v_off = u_off + (width / 2) * (height / 2);
  int uv_idx;

  // Luminocity and color channels
  uint8_t y, u, v;
  int f_dim = 40; // Preset dimming factor

  // 6 o'clock triangle
  double fi = 2 * M_PI / n_sectors; // Triangle top angle
  int height_mid = height / 2;
  int width_mid = width / 2;
  int width_length; // The length of the bottom half-side of the main triangle
  int h_dst;        // Destination pixel height
  int w_dst;        // Destination pixel width

  /****************************************************************************
   * >>====---------------           PIPELINE           ---------------====<< *
   ****************************************************************************/

  // 0. Enforce out of place execution of the filter's algorithm
  if (yuvbuf_in == yuvbuf_out) {
    uint8_t *yuvbuf_in_2 = (uint8_t *)malloc(buf_size);
    assert(yuvbuf_in_2);
    memmove(yuvbuf_in_2, yuvbuf_in, buf_size);
    yuvbuf_in = yuvbuf_in_2;
  }

  // 1. Dim the image
  dim_image(yuvbuf_out, width, height, f_dim);

  /* 2. SHRINK: The main triangle to create the basic 6 o' clock kaleidoscope
  triangle. Construction of the triangle is performed in a bottom up fashion.
  Vertical shrinking is implemented by skipping every second pixel in the height
  dimension of the original triangle. A schematic of the geometry is included in
  the README.md */
  for (int h = height - 1; h >= 0; h -= 2) {

    // Split the base of the triangle in two halfs by using half of top angle
    width_length = h * tan(fi / 2);

    // Horizontal shrinking by skipping every second pixel in  width dimension
    for (int w = width_mid - width_length; w <= width_mid + width_length;
         w += 2) {

      /* Get the YUV values of the pixel from the main triangle. These are 
      bright since they are comming from the input image */
      uv_idx = (h / 2) * (width / 2) + (w / 2);
      y = yuvbuf_in[h * width + w];
      u = yuvbuf_in[u_off + uv_idx];
      v = yuvbuf_in[v_off + uv_idx];

      // Calculate the coordinates of 6 o'clock triangle's respective position
      h_dst = height_mid + h / 2;
      w_dst = width_mid + (w - width_mid) / 2;

      // Copy pixel value to 6 o'clock triangle
      uv_idx = (h_dst / 2) * (width / 2) + (w_dst / 2);
      yuvbuf_out[h_dst * width + w_dst] = y;
      yuvbuf_out[u_off + uv_idx] = u;
      yuvbuf_out[v_off + uv_idx] = v;

      // 3. ROTATE: Clone the pixel to all other sectors by rotation
      clone_pixel(yuvbuf_out, width, height, w_dst, h_dst, fi, n_sectors, y, u,
                  v, u_off, v_off);
    } // End inner for
  }   // End outer for
}

/******************************************************************************
 *                              Helper functions                              *
 ******************************************************************************/

/**
 * clone_pixel
 *
 * Copies a pixel to all requested sectors by pivoting around the center of the
 * image
 */
void clone_pixel(uint8_t *yuvbuf, int width, int height, int w, int h,
                 double fi, int n_sectors, int y, int u, int v, int u_off,
                 int v_off) {
  double r;     // Radious of a pixel from the center of the image
  double theta; // Angle in respect to the center of the image
  int h_dst;    // Destination pixel height coordinate
  int w_dst;    // Destination pixel width coordinate
  int uv_idx;   // Index alignment within the YUV frame

  // Shift the coordinates origin from top left to the middle of the image
  h = h - height / 2;
  w = w - width / 2;

  // Convert the pixel's cartesian coordinates (h, w) to polar (r, θ)
  r = sqrt(w * w + h * h);
  theta = atan2(h, w);

  /* For all the sectors except the first one (multiplied by a factor of 2 for
  enhancing image smoothness */
  for (int s = 1; s < n_sectors * 2; ++s) {

    // Add the rotation factor to the angle while covering all quadrants
    theta = (s % 2 != 0) ? theta + fi * s : theta - fi * s;

    // Convert coordinates from polat back to cartesian readjusting the origin
    h_dst = r * sin(theta) + height / 2;
    w_dst = r * cos(theta) + width / 2;

    // Set new pixel value at the rotated position
    for (int i = 0; i < 3; ++i) {
      uv_idx = (h_dst / 2) * (width / 2) + (w_dst / 2);
      yuvbuf[h_dst * width + w_dst] = y;
      yuvbuf[u_off + uv_idx] = u;
      yuvbuf[v_off + uv_idx] = v;

      // Painting some neighbor pixels as well for smoothing
      if (i % 2 == 0) {
        --h_dst;
        --w_dst;
      } else if (i % 3 == 0) {
        --h_dst;
        ++w_dst;
      } else {
        ++h_dst;
        --w_dst;
      } // End if / else

      // Check we do not cross the image boundaries
      if (h_dst < 1 || h_dst > height - 1 || w_dst < 1 || w_dst > width - 1) {
        break;
      }
    } // End inner for
  }   // End outer for
}

/**
 * dim_image
 *
 * Dim all pixels of the input image according to the dimming factor by adjusting
 * the luminance channel (Y) value
 */
void dim_image(uint8_t *yuvbuf, int width, int height, int f_dim) {
  for (int h = 0; h < height; ++h) {
    for (int w = 0; w < width; ++w) {
      yuvbuf[h * width + w] -= f_dim;
    }
  }
}
