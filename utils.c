#include <assert.h>
#include <jpeglib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

static void i420_to_jpeg_ycbcr(uint8_t *destbuf, int width, int height,
                               const uint8_t *srcbuf) {
  int h, w, dest_idx, u_off, v_off, uv_idx;

  u_off = width * height;
  v_off = u_off + (width / 2) * (height / 2);

  for (h = 0; h < height; h++) {
    for (w = 0; w < width; w++) {
      dest_idx = h * width * 3 + w * 3;
      uv_idx = (h / 2) * (width / 2) + (w / 2);
      destbuf[dest_idx + 0] = srcbuf[h * width + w];
      destbuf[dest_idx + 1] = srcbuf[u_off + uv_idx];
      destbuf[dest_idx + 2] = srcbuf[v_off + uv_idx];
    }
  }
}

static void jpeg_ycbcr_to_i420(uint8_t *destbuf, int width, int height,
                               const uint8_t *srcbuf) {
  int h, w, dest_idx, u_off, v_off, uv_idx;

  u_off = width * height;
  v_off = u_off + (width / 2) * (height / 2);

  for (h = 0; h < height; h++) {
    for (w = 0; w < width; w++) {
      dest_idx = h * width * 3 + w * 3;
      uv_idx = (h / 2) * (width / 2) + (w / 2);
      destbuf[h * width + w] = srcbuf[dest_idx + 0];
      destbuf[u_off + uv_idx] = srcbuf[dest_idx + 1];
      destbuf[v_off + uv_idx] = srcbuf[dest_idx + 2];
    }
  }
}

void read_jpeg_to_i420(const char *inpath, int *_width, int *_height,
                       uint8_t **_yuvbuf) {
  const int num_components = 3;
  FILE *infile;
  int i, width, height;
  uint8_t *yuvbuf;
  uint8_t *jpeg_ycbcr_buf;
  struct jpeg_decompress_struct dinfo;
  struct jpeg_error_mgr jerr;

  infile = fopen(inpath, "rb");
  assert(infile);

  dinfo.err = jpeg_std_error(&jerr);
  jpeg_create_decompress(&dinfo);
  jpeg_stdio_src(&dinfo, infile);
  jpeg_read_header(&dinfo, TRUE);

  dinfo.out_color_space = JCS_YCbCr;
  jpeg_start_decompress(&dinfo);

  width = dinfo.output_width;
  height = dinfo.output_height;

  jpeg_ycbcr_buf = malloc(width * height * num_components);
  assert(jpeg_ycbcr_buf);

  while (dinfo.output_scanline < height) {
    JSAMPROW scanlines[4];
    for (i = 0; i < dinfo.rec_outbuf_height; i++)
      scanlines[i] =
          &jpeg_ycbcr_buf[(dinfo.output_scanline + i) * width * num_components];
    jpeg_read_scanlines(&dinfo, scanlines, dinfo.rec_outbuf_height);
  }

  jpeg_finish_decompress(&dinfo);
  jpeg_destroy_decompress(&dinfo);
  fclose(infile);

  yuvbuf = malloc(width * height * 3 / 2);
  assert(yuvbuf);

  jpeg_ycbcr_to_i420(yuvbuf, width, height, jpeg_ycbcr_buf);
  free(jpeg_ycbcr_buf);

  *_width = width;
  *_height = height;
  *_yuvbuf = yuvbuf;
}

void write_i420_to_jpeg(const uint8_t *yuvbuf, int width, int height,
                        const char *outpath) {
  const int num_components = 3;
  FILE *outfile;
  struct jpeg_compress_struct cinfo;
  struct jpeg_error_mgr jerr;
  size_t bytes_written;
  int i;
  uint8_t *jpeg_ycbcr_buf;
  uint8_t *outbuffer = NULL;
  unsigned long outlen = 0;
  JSAMPARRAY scanlines;

  cinfo.err = jpeg_std_error(&jerr);
  jpeg_create_compress(&cinfo);
  jpeg_mem_dest(&cinfo, &outbuffer, &outlen);

  cinfo.image_width = width;
  cinfo.image_height = height;
  cinfo.input_components = num_components;
  cinfo.in_color_space = JCS_YCbCr;

  jpeg_set_defaults(&cinfo);
  jpeg_set_quality(&cinfo, 92, TRUE);
  jpeg_start_compress(&cinfo, TRUE);

  jpeg_ycbcr_buf = malloc(width * height * num_components);
  assert(jpeg_ycbcr_buf);

  i420_to_jpeg_ycbcr(jpeg_ycbcr_buf, width, height, yuvbuf);

  scanlines = malloc(sizeof(JSAMPROW) * height);
  assert(scanlines);

  for (i = 0; i < height; i++)
    scanlines[i] = (JSAMPROW)&jpeg_ycbcr_buf[width * num_components * i];

  jpeg_write_scanlines(&cinfo, scanlines, height);
  jpeg_finish_compress(&cinfo);

  free(scanlines);

  outfile = fopen(outpath, "wb");
  assert(outfile);

  bytes_written = fwrite(outbuffer, 1, outlen, outfile);
  assert(bytes_written == outlen);

  fclose(outfile);

  jpeg_destroy_compress(&cinfo);

  free(outbuffer);
  free(jpeg_ycbcr_buf);
}
