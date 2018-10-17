#include "display.h"
#include "defs.h"

/*
 * Drawing
 */

#ifndef _swap_int8_t
#define _swap_int8_t(a, b) { int8_t t = a; a = b; b = t; }
#endif

/**************************************************************************/
/*!
   @brief    Write a line.  Bresenham's algorithm - thx wikpedia
    @param    x0  Start point x coordinate
    @param    y0  Start point y coordinate
    @param    x1  End point x coordinate
    @param    y1  End point y coordinate
*/
/**************************************************************************/
void display_draw_line(Display* self, int8_t x0, int8_t y0, int8_t x1, int8_t y1)
{
    int8_t steep = abs(y1 - y0) > abs(x1 - x0);
    if (steep) {
        _swap_int8_t(x0, y0);
        _swap_int8_t(x1, y1);
    }

    if (x0 > x1) {
        _swap_int8_t(x0, x1);
        _swap_int8_t(y0, y1);
    }

    uint8_t dx, dy;
    dx = x1 - x0;
    dy = abs(y1 - y0);

    int8_t err = dx / 2;
    int8_t ystep;

    if (y0 < y1) {
        ystep = 1;
    } else {
        ystep = -1;
    }

    // We have to be carefull with the loop here because the last column on the
    // screen is at x=127, which is the maximum value that an 8 bit value can
    // take. So, a for-loop condition like x0<=127 will always be true.
    do {

      if (steep) {
          display_impl_draw_pixel(&self->_impl, y0, x0);
      } else {
          display_impl_draw_pixel(&self->_impl, x0, y0);
      }
      err -= dy;
      if (err < 0) {
          y0 += ystep;
          err += dx;
      }

    } while (x0++ < x1);

    /*for (; x0<=x1; x0++) {
        if (steep) {
            display_impl_draw_col_pixel(&self->_impl, y0, x0);
        } else {
            display_impl_draw_col_pixel(&self->_impl, x0, y0);
        }
        err -= dy;
        if (err < 0) {
            y0 += ystep;
            err += dx;
        }
    }*/
}

#ifndef __AVR

void display_draw_col_line(Display* self,
                           int8_t x0, int8_t y0,
                           int8_t x1, int8_t y1,
                           uint8_t colour)
{
    int8_t steep = abs(y1 - y0) > abs(x1 - x0);
    if (steep) {
        _swap_int8_t(x0, y0);
        _swap_int8_t(x1, y1);
    }

    if (x0 > x1) {
        _swap_int8_t(x0, x1);
        _swap_int8_t(y0, y1);
    }

    uint8_t dx, dy;
    dx = x1 - x0;
    dy = abs(y1 - y0);

    int8_t err = dx / 2;
    int8_t ystep;

    if (y0 < y1) {
        ystep = 1;
    } else {
        ystep = -1;
    }

    // We have to be carefull with the loop here because the last column on the
    // screen is at x=127, which is the maximum value that an 8 bit value can
    // take. So, a for-loop condition like x0<=127 will always be true.
    do {

      if (steep) {
          display_impl_draw_col_pixel(&self->_impl, y0, x0, colour);
      } else {
          display_impl_draw_col_pixel(&self->_impl, x0, y0, colour);
      }
      err -= dy;
      if (err < 0) {
          y0 += ystep;
          err += dx;
      }

    } while (x0++ < x1);

    /*for (; x0<=x1; x0++) {
        if (steep) {
            display_impl_draw_col_pixel(&self->_impl, y0, x0);
        } else {
            display_impl_draw_col_pixel(&self->_impl, x0, y0);
        }
        err -= dy;
        if (err < 0) {
            y0 += ystep;
            err += dx;
        }
    }*/
}

#endif
