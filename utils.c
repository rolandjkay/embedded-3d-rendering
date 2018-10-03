#include <stdint.h>

/*
 * From Hacker's Delight 7-3
 * https://books.google.co.uk/books?id=iBNKMspIlqEC&lpg=PP1&pg=RA1-SL20-PA8&redir_esc=y&hl=fr#v=onepage&q&f=false
 */
static void transpose8(uint8_t A[8], uint8_t B[8])
{
  uint32_t x, y, t;

  // Load the array and pack it into x and y.
  x = (A[0]<<24)    | (A[1]<<16)   | (A[2]<<8) | A[3];
  y = (A[4]<<24)    | (A[5]<<16)   | (A[6]<<8) | A[7];

  t = (x ^ (x >> 7)) & 0x00AA00AA; x = x ^ t ^ (t << 7);
  t = (y ^ (y >> 7)) & 0x00AA00AA; y = y ^ t ^ (t << 7);

  t = (x ^ (x >> 14)) & 0x0000CCCC; x = x ^ t ^ (t << 14);
  t = (y ^ (y >> 14)) & 0x0000CCCC; y = y ^ t ^ (t << 14);

  t = (x & 0xF0F0F0F0) |  ((y >>4) & 0x0F0F0F0F);
  y = ((x << 4) & 0xF0F0F0F0) | (y & 0x0F0F0F0F);
  x = t;

  B[0]=x>>24;   B[1]=x>>16;   B[2]=x>>8;  B[3]=x;
  B[4]=y>>24;   B[5]=y>>16;   B[6]=y>>8;  B[7]=y;
}

/*
 * Invert 'endianess' of a single byte.
 */
static uint8_t reverse(uint8_t b) {
   b = (b & 0xF0) >> 4 | (b & 0x0F) << 4;
   b = (b & 0xCC) >> 2 | (b & 0x33) << 2;
   b = (b & 0xAA) >> 1 | (b & 0x55) << 1;
   return b;
}


void util_horiz_pixel_buf_to_vert(const unsigned char input[1024],
                                  unsigned char output[1024])
{
  for (int row = 0; row < 64; row += 8)
  {
    unsigned char A[8], B[8];

    for (int col = 0; col < 16; ++col)
    {
      for (int i = 0; i < 8; ++i)
      {
         A[i] = input[(row<<4) + (i << 4) + col];
      }

      transpose8(A, B);

      // We really want an 'inverse' transpose; that is a traspose across the
      // Digonal that runs bottom-left to top-right. So, we have to reverse
      // the order of the bits after doing a normal transpose.
      for (int i = 0; i < 8; ++i)
      {
        output[(row<<4) + (col << 3) + i] = reverse(B[i]);
      }
    }
  }
}

void util_vert_pixel_buf_to_horiz(const unsigned char input[1024],
                                  unsigned char output[1024])
{
  // It's a self inverse ;)
  util_horiz_pixel_buf_to_vert(input, output);
}
