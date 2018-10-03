#include <stdio.h>
#include <stdint.h>
#include "sprites.h"
#include "../utils.h"


/*
 * Transpose 8x8 blocks of each sprite to match the screen buffer format
 * of the SSD1306.
 */
int main()
{
  unsigned char buf[1024];

  util_horiz_pixel_buf_to_vert(cat_128_64, buf);

  FILE* f = fopen("sprites.c", "w");
  fprintf(f, "const unsigned char cat_128_64[] = {\n");
  unsigned char* p = &buf[0];
  for (int i = 0; i < 64; ++i)
  {
    for (int j = 0; j < 16; ++j, ++p)
    {
      fprintf(f, "0x%02x,", *p);
    }
    fprintf(f, "\n");
  }
  fprintf(f, "};\n");
  fclose(f);
}
