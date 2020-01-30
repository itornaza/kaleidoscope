#ifndef UTILS_H
#define UTILS_H

#include <stdint.h>

void read_jpeg_to_i420(const char *inpath, int *_width, int *_height,
                       uint8_t **_yuvbuf);

void write_i420_to_jpeg(const uint8_t *yuvbuf, int width, int height,
                        const char *outpath);

#endif /* UTILS_H */
