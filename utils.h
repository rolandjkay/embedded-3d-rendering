#ifndef _UTILS_H
#define _UTILS_H

void util_horiz_pixel_buf_to_vert(const unsigned char input[1024],
                                  unsigned char output[1024]);
void util_vert_pixel_buf_to_horiz(const unsigned char input[1024],
                                  unsigned char output[1024]);

#endif
