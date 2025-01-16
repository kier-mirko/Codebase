#ifndef BASE_IMAGE_H
#define BASE_IMAGE_H

#include "base.h"
#include "string.h"

/* TODO: handmade image loading maybe? */
/* #define STB_IMAGE_IMPLEMENTATION */
/* #include "extern/stb_image.h" */
/* Failes to include `<string.h>` */

inline U8 *loadImg(String8 path, I32 *width, I32 *height, I32 *componentXpixel) {
  /* return stbi_load((char *)path.str, width, height, componentXpixel, 0); */
  return 0;
}

inline void destroyImg(U8 *imgdata) {
  /* stbi_image_free(imgdata); */
}

#endif
