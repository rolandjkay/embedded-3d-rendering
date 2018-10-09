/*
 * Implementation of display for the SSD1306 screen.
 */
#ifndef _SSD1306_DISPLAY_IMPL_H
#define _SSD1306_DISPLAY_IMPL_H

#define SSD1306_128_64

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "ssd1306.h"

// Size in bytes of the backing buffer for the display (1 bit per pixel)
#define SSD1306_SCREEN_BUFFER_SIZE SSD1306_LCDWIDTH*SSD1306_LCDHEIGHT / 8

typedef struct
{
  uint8_t _screen_buffer[SSD1306_SCREEN_BUFFER_SIZE];
} DisplayImpl;

#define DISPLAY_IMPL_INIT {0}

int display_impl_init(DisplayImpl* self);

static inline int display_impl_get_width(DisplayImpl* display) { return SSD1306_LCDWIDTH; }
static inline int display_impl_get_height(DisplayImpl* display) { return SSD1306_LCDHEIGHT; }

/*
 * Drawing
 */
static inline void display_impl_cls(DisplayImpl* self)
{
  memset(&self->_screen_buffer[0], 0, sizeof(self->_screen_buffer));
}

void display_impl_draw_pixel(DisplayImpl* self, int16_t x, int16_t y);
//uint8_t display_impl_pixel_colour(Display* self, int16_t x, int16_t y);

void display_impl_show(DisplayImpl* self);

/*
 * Access to screen buffer
 */
static inline uint8_t* display_impl_get_buffer(DisplayImpl* self)
{
  return &self->_screen_buffer[0];
}

static inline void display_impl_release_buffer(DisplayImpl* self) {}

#endif
