#include "my_library.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * my_crazy_function
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

void my_function(uint8_t *yuvbuf_in, int width, int height,
                 uint8_t *yuvbuf_out) {

  //-----------------------
  // Local variables
  //-----------------------

  // Trigonometry
  int n_sectors = 12;               // Number of sectors of the kaleidoscope
  double fi = 2 * M_PI / n_sectors; // Triangle top angle

  // Offsets for indices of the YUV sections of the frame
  int u_off = width * height;
  int v_off = u_off + (width / 2) * (height / 2);
  int uv_idx;

  // Coordinates for the bottom triangle
  int h_dst, w_dst;
  int height_mid = height / 2;
  int width_mid = width / 2;

  // Luminocity and colors values
  uint8_t y, u, v;

  //-----------------------
  // Pipeline
  //-----------------------

  // 0. Copy input image to another input and an output image 
  int buf_size = width * height * 3 / 2;
  uint8_t *yuvbuf_in_2 = (uint8_t *) malloc(buf_size);
  memmove(yuvbuf_in_2, yuvbuf_in, buf_size);
  memmove(yuvbuf_out, yuvbuf_in, buf_size);

  // 1. Dim the image
  dim_image(yuvbuf_out, width, height);

  // 2. Shrink the main triangle to the bottom
  // For every second pixel in the height dimension
  for (int h = 0; h < height; h += 2) {

    // Adjust h dependent variables
    int width_length = h * tan(fi / 2);

    // 2.1 Print the right normal part of the triangle
    // For every second pixel in the width dimension
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

      clone_sectors(yuvbuf_out, width, height, w_dst, h_dst, fi, n_sectors, y, u, v);
    } // End inner for - right triangle

    // 2.2 Print the left normal part of the triangle
    // For every second pixel in the width dimension
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

      clone_sectors(yuvbuf_out, width, height, w_dst, h_dst, fi, n_sectors, y, u, v);
    } // End inner for - left triangle
  } // End outer for
}

/**
 *  clone_sectors
 *
 *  Cppies every pixel of the scaled down triangle to the rest of the sectors
 */
void clone_sectors(uint8_t *yuvbuf, int width, int height, int w, int h,
                   double fi, int n_sectors, int y, int u, int v) {

  // 1. Convert cartesian to polar coordinates in respect to image middle
  h = h - height / 2;
  w = w - width / 2;
  double r = sqrt(w * w + h * h);
  double theta = atan2(h, w);

  // For all the sectors except the first one
  for (int s = 1; s < n_sectors; ++s) {

    // 2. Add the rotation factor to the angle and cover all quadrants!
    if (s % 2 != 0) {
      theta -= fi * s;
    } else {
      theta += fi * s;
    }

    // 3, Convert from polar to cartesian and print pixel at new position
    int h_dst = r * sin(theta) + height / 2;
    int w_dst = r * cos(theta) + width / 2;

    // 4. Write the pixels to offseted sector
    int u_off = width * height;
    int v_off = u_off + (width / 2) * (height / 2);
    int uv_idx = (h_dst / 2) * (width / 2) + (w_dst / 2);
    yuvbuf[h_dst * width + w_dst] = y;
    yuvbuf[u_off + uv_idx] = u;
    yuvbuf[v_off + uv_idx] = v;
  }
}

/**
 *  dim_image
 * 
 *  For all true pixels in the buffer use the mask to dim set pixels
 */
void dim_image(uint8_t *yuvbuf, int width, int height) {
  for (int h = 0; h < height; ++h) {
    for (int w = 0; w < width; ++w) {
      yuvbuf[h * width + w] -= 40;
    }
  }
}
