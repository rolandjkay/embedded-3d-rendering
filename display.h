#ifndef _DISPLAY_H
#define _DISPLAY_H

#ifdef __AVR__
#  include "avr/ssd1306/display_impl_ssd1306.h"
  //define WITHARCH(x) x ## _ssd1306
#else
#  include "macos/sdl/display_impl_sdl.h"
  //define WITHARCH(x) x ## _sdl
#endif

#include "stdint.h"

typedef struct
{
  DisplayImpl _impl;
} Display;

#define DISPLAY_INIT { DISPLAY_IMPL_INIT }


/*
 * Ctor and dtor
 */
static inline int display_init(Display* self)
{
  return display_impl_init(&self->_impl);
}

static inline int display_get_width(Display* self)
{
  return display_impl_get_width(&self->_impl);
}

static inline int display_get_height(Display* self)
{
  return display_impl_get_height(&self->_impl);
}

/*
 * Drawing
 */
static inline void display_cls(Display* self)
{
  display_impl_cls(&self->_impl);
}

static inline void display_show(Display* self)
{
  display_impl_show(&self->_impl);
}

void display_draw_line(Display* self, int8_t x0, int8_t y0, int8_t x1, int8_t y1);

static inline void display_draw_pixel(Display* self, int8_t x, int8_t y)
{
  display_impl_draw_pixel(&self->_impl, x, y);
}

#ifndef __AVR
  void display_draw_col_line(Display* self, int8_t x0, int8_t y0, int8_t x1, int8_t y1);
  //void display_draw_col_pixel(Display* self, int16_t x, int16_t y);
#endif

/*
 * Access to virtual buffer
 */
static inline uint8_t* display_get_buffer(Display* self)
{
  return display_impl_get_buffer(&self->_impl);
}

static inline void display_release_buffer(Display* self)
{
  display_impl_release_buffer(&self->_impl);
}

#endif
