#include "my_library.h"

#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void clone_pixel(uint8_t *yuvbuf, int width, int height, int w, int h,
                 double fi, int n_sectors, int y, int u, int v, int u_off,
                 int v_off);

static void dim_image(uint8_t *yuvbuf, int width, int height, int f_dim);

/**
 * kaleidoscope
 *
 * Implements the kaleidoscope effect with an adjustable number of sectors
 */
void kaleidoscope(uint8_t *yuvbuf_in, int width, int height,
                  uint8_t *yuvbuf_out, int n_sectors) {
  /* Indices and offsets within the YUV sections of frame buffers */
  int u_off = width * height;
  int v_off = u_off + (width / 2) * (height / 2);
  int uv_idx;
  int h_dst; /* Destination pixel height */
  int w_dst; /* Destination pixel width */

  /* Luminocity and color channels */
  uint8_t y, u, v;
  int f_dim = 2; /* Preset dimming factor */

  /* Kaleidoscope main triangle geometry */
  double fi = 2 * M_PI / n_sectors; /* Triangle top angle */
  int height_mid = height / 2;
  int width_mid = width / 2;
  int base_len; /* Half the length of the bottom side of the triangle */

  /* Image buffers with enforced out-place implementation policy */
  int buf_size = width * height * 3 / 2;
  memmove(yuvbuf_out, yuvbuf_in, buf_size);
  uint8_t *yuvbuf_in_2 = NULL;
  if (yuvbuf_in == yuvbuf_out) {
    yuvbuf_in_2 = (uint8_t *)malloc(buf_size);
    assert(yuvbuf_in_2);
    memmove(yuvbuf_in_2, yuvbuf_in, buf_size);
    yuvbuf_in = yuvbuf_in_2;
  }

  /****************************************************************************
   * >>====---------------           PIPELINE           ---------------====<< *
   ****************************************************************************/

  /* 1. DIM the image */
  dim_image(yuvbuf_out, width, height, f_dim);

  /* 2. SHRINK the main image triangle to create the basic 6 o' clock
  kaleidoscope triangle. Construction of the triangle is performed in a bottom
  up fashion. Vertical shrinking is implemented by skipping every second pixel
  in the height dimension of the original triangle. A schematic of the geometry
  is included in the README.md */
  for (int h = height - 1; h >= 0; h -= 2) {
    base_len = h * tan(fi / 2);

    /* Shrink horizontaly by skipping every second pixel in width dimension */
    for (int w = width_mid - base_len; w <= width_mid + base_len; w += 2) {
      /* Get the YUV values of the pixel from the main triangle. These are
      still bright since they are comming straight from the input image */
      uv_idx = (h / 2) * (width / 2) + (w / 2);
      y = yuvbuf_in[h * width + w];
      u = yuvbuf_in[u_off + uv_idx];
      v = yuvbuf_in[v_off + uv_idx];

      /* Get the coordinates of the 6 o'clock triangle's respective position */
      h_dst = height_mid + h / 2;
      w_dst = width_mid + (w - width_mid) / 2;

      /* 3. ROTATE and copy the pixel to the corresponding pixels in all
      sectors */
      clone_pixel(yuvbuf_out, width, height, w_dst, h_dst, fi, n_sectors, y, u,
                  v, u_off, v_off);
    } /* End inner for */
  }   /* End outer for */

  /* Clean up buffers */
  if (yuvbuf_in_2 != NULL) {
    yuvbuf_in = yuvbuf_out;
    free(yuvbuf_in_2);
  }
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
static void clone_pixel(uint8_t *yuvbuf, int width, int height, int w, int h,
                 double fi, int n_sectors, int y, int u, int v, int u_off,
                 int v_off) {
  double r;     /* Radious of a pixel from the center of the image */
  double theta; /* Angle in respect to the center of the image */
  int h_dst;    /* Destination pixel height coordinate */
  int w_dst;    /* Destination pixel width coordinate */
  int uv_idx;   /* Index alignment within the YUV frame */

  /* Shift the coordinates origin from the top left to the middle of the image */
  h = h - height / 2;
  w = w - width / 2;

  /* Convert the pixel's coordinates from cartesian (h, w) to polar (r, θ) */
  r = sqrt(w * w + h * h);
  theta = atan2(h, w);

  for (int s = 0; s < n_sectors; ++s) {
    /* Add the rotation factor to the base angle while covering all quadrants */
    theta = (s % 2 != 0) ? theta + fi * s : theta - fi * s;

    /* Convert coordinates from polar back to cartesian while reseting origin */
    h_dst = r * sin(theta) + height / 2;
    w_dst = r * cos(theta) + width / 2;

    /* Paint the pixel at the rotated position plus the top, bottom, left and 
    right neighbors for image smoothness */
    for (int i = 0; i < 5; ++i) {
      uv_idx = (h_dst / 2) * (width / 2) + (w_dst / 2);
      yuvbuf[h_dst * width + w_dst] = y;
      yuvbuf[u_off + uv_idx] = u;
      yuvbuf[v_off + uv_idx] = v;
      if (i == 0) {
        h_dst--;
      } else if (i == 1) {
        h_dst += 2;
      } else if (i == 2) {
        h_dst--;
        w_dst--;
      } else {
        w_dst += 2;
      } /* End if/else */

      /* Check we do not cross the image boundaries */
      if (h_dst < 1 || h_dst > height - 1 || w_dst < 1 || w_dst > width - 1) {
        break;
      } /* End if */
    }   /* End inner for */
  }     /* End outer for */
}

/**
 * dim_image
 *
 * Dim all pixels of the input image according to the dimming factor by
 * adjusting the luminance channel (Y) value
 */
static void dim_image(uint8_t *yuvbuf, int width, int height, int f_dim) {
  for (int h = 0; h < height; ++h) {
    for (int w = 0; w < width; ++w) {
      yuvbuf[h * width + w] /= f_dim;
    }
  }
}
