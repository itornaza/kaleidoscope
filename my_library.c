#include "my_library.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * my_crazy_function
 *
 * Please disregard in code review!
 *
 * Designed for understanding of the YUV color space and creates a slightly
 * different radial kaleidoskope effect
 */
void my_crazy_function(uint8_t *yuvbuf_in, int width, int height,
                       uint8_t *yuvbuf_out) {
  memmove(yuvbuf_out, yuvbuf_in, width * height * 3 / 2);

  double n_sectors = 10.0;          // The numner of sectors of the kaleidoscope
  double theta = 0.0;               // θ is the running angle
  double theta_inc = 0.00001;       // θ increment step
  double fi = 2 * M_PI / n_sectors; // φ is the sector angle
  int r_max = width / 4;            // Kaleidoskope max range from image center
  int r_inc = width / 1000;         // radious increment step

  while (fi - theta >= 0.001) {

    // For radial pixels starting from origin to kaleidoskope outer boundary
    for (int r = 0; r < r_max; r += r_inc) {

      // 1. Convert polar to cartesian coordinates
      int h = r * sin(theta);
      int w = r * cos(theta);

      // 2. Shift origin from the top left corner to the middle of the image
      h += height / 2;
      w += width / 2;

      // 3. Adjust the indices of the YUV sections of the frame
      int u_off = width * height;
      int v_off = u_off + (width / 2) * (height / 2);
      int uv_idx = (h / 2) * (width / 2) + (w / 2);

      // 4. Get the YUV values of the pixel
      int y = yuvbuf_out[h * width + w];
      int u = yuvbuf_out[u_off + uv_idx];
      int v = yuvbuf_out[v_off + uv_idx];

      // 5. Assign pixel YUV value to the rest of the arcs
      for (int s = 0; s <= n_sectors; ++s) {

        // 5.1 Convert polar to cartesian
        h = r * sin(theta + s * fi) + height / 2;
        w = r * cos(theta + s * fi) + width / 2;

        // 5.2 Adjust the indices
        uv_idx = (h / 2) * (width / 2) + (w / 2);

        // 5.3 Set the YUV values of the pixel
        yuvbuf_out[h * width + w] = y;
        yuvbuf_out[u_off + uv_idx] = u;
        yuvbuf_out[v_off + uv_idx] = v;
      }
    }
    theta += theta_inc;
  }
}

/**
 * kaleidoscope
 *
 * Implements the kaleidoscope effect with an adjustable number of sectors
 */
void kaleidoscope(uint8_t *yuvbuf_in, int width, int height,
                  uint8_t *yuvbuf_out, int n_sectors) {

  //---------------------------------------------------------------------------
  // Local variables
  //---------------------------------------------------------------------------

  // Trigonometry
  double fi = 2 * M_PI / n_sectors; // Triangle top angle

  // Offsets for indices of the YUV sections within the frame
  int u_off = width * height;
  int v_off = u_off + (width / 2) * (height / 2);
  int uv_idx;

  // Coordinates for the bottom triangle
  int height_mid = height / 2;
  int width_mid = width / 2;
  int width_length; // The length of the bottom half-side of the main triangle
  int h_dst;        // Destination pixel height
  int w_dst;        // Destination pixel width

  // Luminocity and color channels
  uint8_t y, u, v;
  int f_dim = 40; // Dimming factor

  //---------------------------------------------------------------------------
  // Buffers
  //---------------------------------------------------------------------------

  int buf_size = width * height * 3 / 2;
  uint8_t *yuvbuf_in_2 = (uint8_t *)malloc(buf_size);
  memmove(yuvbuf_in_2, yuvbuf_in, buf_size);
  memmove(yuvbuf_out, yuvbuf_in, buf_size);

  //---------------------------------------------------------------------------
  // Pipeline
  //---------------------------------------------------------------------------

  // 1. Dim the image, everything copied from the orig will be bright again
  dim_image(yuvbuf_out, width, height, f_dim);

  // 2. Shrink the main triangle at the bottom to create the basic part

  // For every second pixel in the height dimension
  for (int h = 0; h < height; h += 2) {

    // Adjust h dependent variables
    width_length = h * tan(fi / 2);

    // 2.1 Print the right normal part of the triangle

    // For every second pixel in the main triangle's width dimension
    for (int w = width_mid; w <= width_mid + width_length; w += 2) {

      // Adjust colorspace indices
      uv_idx = (h / 2) * (width / 2) + (w / 2);

      // Get the YUV values of the pixel from the original triangle
      y = yuvbuf_in_2[h * width + w];
      u = yuvbuf_in_2[u_off + uv_idx];
      v = yuvbuf_in_2[v_off + uv_idx];

      // Copy every second row pixel of the original triangle to the bottom
      h_dst = height_mid + h / 2;
      w_dst = width_mid + (w - width_mid) / 2;

      // Adjust colorspace indices
      uv_idx = (h_dst / 2) * (width / 2) + (w_dst / 2);

      // Assign pixel values to bottom triangle
      yuvbuf_out[h_dst * width + w_dst] = y;
      yuvbuf_out[u_off + uv_idx] = u;
      yuvbuf_out[v_off + uv_idx] = v;

      // 3. Clone the pixel to all other sectors
      clone_pixel(yuvbuf_out, width, height, w_dst, h_dst, fi, n_sectors, y, u,
                  v, u_off, v_off);
    } // End inner for - right triangle

    // 2.2 Print the left normal part of the triangle

    // For every second pixel in the main triangle's width dimension
    for (int w = width_mid; w >= width_mid - width_length; w -= 2) {

      // Adjust colorspace indices
      uv_idx = (h / 2) * (width / 2) + (w / 2);

      // Get the YUV values of the pixel from the original triangle
      y = yuvbuf_in_2[h * width + w];
      u = yuvbuf_in_2[u_off + uv_idx];
      v = yuvbuf_in_2[v_off + uv_idx];

      // Copy every second row pixel of the original triangle to the bottom
      h_dst = height_mid + (h / 2);
      w_dst = w + (width_mid - w) / 2;

      // Adjust colorspace indices
      uv_idx = (h_dst / 2) * (width / 2) + (w_dst / 2);

      // Assign pixel values to bottom triangle
      yuvbuf_out[h_dst * width + w_dst] = y;
      yuvbuf_out[u_off + uv_idx] = u;
      yuvbuf_out[v_off + uv_idx] = v;

      // 3. Clone the pixel to all other sectors
      clone_pixel(yuvbuf_out, width, height, w_dst, h_dst, fi, n_sectors, y, u,
                  v, u_off, v_off);
    } // End inner for - left triangle
  }   // End outer for
}

/**
 *  clone_sectors
 *
 *  Cppies every pixel of the scaled down triangle to the rest of the sectors
 */
void clone_pixel(uint8_t *yuvbuf, int width, int height, int w, int h,
                 double fi, int n_sectors, int y, int u, int v, int u_off,
                 int v_off) {
  double r;     // Radious of a pixel from the center of the image
  double theta; // Angle in respect to the center of the image
  int h_dst;    // Destination pixel height coordinate
  int w_dst;    // Destination pixel width coordinate
  int uv_idx;   // Index alignment within the YUV frame

  // 1. Shift origin from top left to the middle of the image
  h = h - height / 2;
  w = w - width / 2;

  // 2. Get the polar coordinates in respect to the middle of the image
  r = sqrt(w * w + h * h);
  theta = atan2(h, w);

  // For all the sectors but the first one with a factor of 2 for
  // optimisation on color smoothness in conjunction with item 6 below
  for (int s = 1; s < n_sectors * 2; ++s) {

    // 3. Add the rotation factor to the angle and cover all quadrants!
    theta = (s % 2 != 0) ? theta + fi * s : theta - fi * s;

    // 4. Convert from polar to cartesian and print pixel at new position
    h_dst = r * sin(theta) + height / 2;
    w_dst = r * cos(theta) + width / 2;

    // 6. Write each pixel in other sectors at the respective place and in
    //    close proximity to avoid atan2 pitholes
    for (int i = 0; i < 3; ++i) {
      uv_idx = (h_dst / 2) * (width / 2) + (w_dst / 2);
      yuvbuf[h_dst * width + w_dst] = y;
      yuvbuf[u_off + uv_idx] = u;
      yuvbuf[v_off + uv_idx] = v;
      --h_dst;
      --w_dst;
    } // End inner for
  }   // End outer for
}

/**
 *  dim_image
 *
 *  Dim all pixels of the input buffer by the factor
 */
void dim_image(uint8_t *yuvbuf, int width, int height, int f_dim) {
  for (int h = 0; h < height; ++h) {
    for (int w = 0; w < width; ++w) {
      yuvbuf[h * width + w] -= f_dim;
    }
  }
}
