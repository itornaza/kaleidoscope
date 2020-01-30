## Description

Simple program to load a JPEG file, manipulate the data (YUV I420),
and store the result as a JPEG file.

Depends on libjpeg

## Install libjpeg
- On macOS run `brew install libjpeg`
- On Debian run `sudo apt-get install libjpeg-dev`
- On windows follow the instructions [here](http://gnuwin32.sourceforge.net/packages/jpeg.htm)

## Kaleidoscope video effect

The task is to write a program that can take jpeg as an input, apply the kaleidoscope effect similar to how it is shown in [this video](https://www.youtube.com/watch?v=SbQJKjORQJk) and save the result in a jpeg file. The effect as it is shown in the video has multiple processing stages such as: dimming the background, scaling down the image and then moving the pixels around to create the kaleidoscope effect. It is up to you how far you would like to go reproducing the effect, but the minimum that we expect is to be able to move the pixels around to create the kaleidoscope itself. 

To get you started we have prepared a "skeleton" of the program for you. You need to install libjpeg for it to work. It reads & decodes jpeg file producing video frame in I420 YUV pixels format. Then encodes the I420 video frame back to jpeg file. Your task is to apply the kaleidoscope effect to the decoded I420 video frame. If you have never worked with I420 YUV pixel format here is a [link](https://www.fourcc.org/pixel-format/yuv-i420/) that should give you an idea of how pixels are stored in memory.

## Implementation notes

- Had to include the following library in the libjpeg.h to avoid the error
`/usr/local/include/jpeglib.h:774:3: error: unknown type name 'size_t`

```
#ifndef _STDIO_H_
#include <stdio.h>
#endif
```

- Bytestream explained at the bottom of this wikipedia [article](https://en.wikipedia.org/wiki/YUV)