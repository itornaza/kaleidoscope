## Imageloader

Simple program to load a JPEG file, manipulate the data (YUV I420), and store the result as a JPEG file.

- A kaleidoscope effect is included in my_library

- Depends on libjpeg

## Install libjpeg
- On Debian run `sudo apt-get install libjpeg-dev`
- On macOS run `brew install libjpeg`
- On windows follow the instructions [here](http://gnuwin32.sourceforge.net/packages/jpeg.htm)

## Running the program
Cd to project root and run `make`
Then run `./imageloader <input.jpg> <out.jpg>`

## Kaleidoscope video effect

The kaleidoscope function takes jpeg as an input, apply the kaleidoscope effect similar to how it is shown in [this video](https://www.youtube.com/watch?v=SbQJKjORQJk) and saves the result in a jpeg file. The effect as it is shown in the video has multiple processing stages such as: dimming the background, scaling down the image and then moving the pixels around to create the kaleidoscope effect. 

Our kaleidoscope dims the pixels outside the kalidoscope rim and provides an option as to how many sectors the kaleidoscope may display.

I420 YUV pixel format is explained in this [link](https://www.fourcc.org/pixel-format/yuv-i420/) more visual explanation at the bottom of this wikipedia [article](https://en.wikipedia.org/wiki/YUV).

## Implementation notes

- Had to include the following library in the libjpeg.h to avoid the error
`/usr/local/include/jpeglib.h:774:3: error: unknown type name 'size_t`

```
#ifndef _STDIO_H_
#include <stdio.h>
#endif
```

- The kaleidoscope effect is implemented in the function `kaleidoscope` that is declared `my_library.h` and defined in `my_library.c`. Further, helper functions are used and defined after `kaleidoscope`.