#include "display_impl_sdl.h"
#include "../defs.h"
#include "../errors.h"
#include "../utils.h"
#include <sys/time.h>
#include <SDL2/SDL.h>



int display_impl_init(DisplayImpl* self)
{
  if (SDL_Init( SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0 )
  {
    set_error(ERROR_DISPLAY_HW_ERROR);
    return -1;
  }

  self->_window = NULL;
  self->_renderer = NULL;
  self->_screen = NULL;
  self->_texture = NULL;
  self->_width = 128;
  self->_height = 64;
  memset(self->_pixels, 0, sizeof(self->_pixels));
  memset(self->_buffer, 0, sizeof(self->_buffer));

  SDL_CreateWindowAndRenderer(self->_width, self->_height, SDL_WINDOW_SHOWN,
                              &self->_window, &self->_renderer);

  if (!self->_window || !self->_renderer)
  {
    set_error(ERROR_DISPLAY_HW_ERROR);
    return -1;
  }


  /*
   * We're not allow to create 1 bit per pixel (indexed, or otherwise) textures.
   * So, we use RGB332, which is at least one byte per pixel, and write 0xff
   * for white and 0x00 for black.
   */
  self->_texture = SDL_CreateTexture(self->_renderer,
                                     SDL_PIXELFORMAT_RGB332,
                                     SDL_TEXTUREACCESS_STATIC,
                                     self->_width, self->_height);
  if (!self->_texture)
  {
    set_error(ERROR_DISPLAY_HW_ERROR);
    return -1;
  }

  // Blank display
  display_impl_cls(self);

  return 0;
}

/*
 * Drawing
 */

#ifndef _swap_int16_t
#define _swap_int16_t(a, b) { int16_t t = a; a = b; b = t; }
#endif

void display_impl_cls(DisplayImpl* self)
{
  // Blank display
  memset(self->_pixels, 0, self->_width * self->_height);

  SDL_UpdateTexture(self->_texture, NULL, self->_pixels, self->_width);
}

void display_impl_show(DisplayImpl* self)
{
  SDL_UpdateTexture(self->_texture, NULL,
                    self->_pixels,
                    self->_width);
  SDL_RenderClear(self->_renderer);
  SDL_RenderCopy(self->_renderer, self->_texture, NULL, NULL);
  SDL_RenderPresent(self->_renderer);
}


static uint8_t display_pixel_colour(DisplayImpl* display, int16_t x, int16_t y)
{
  return 0xff;
}

void display_impl_draw_pixel(DisplayImpl* display, int16_t x, int16_t y)
{
  if (x < display->_width && x >= 0 && y < display->_height && y >= 0)
  {
    *(display->_pixels + y * display->_width + x) = display_pixel_colour(display, x, y);
  }
}

// XXX HORRIBLE COPY-PASTE
//
/*void display_impl_draw_col_line(DisplayImpl* display, int16_t x0, int16_t y0, int16_t x1, int16_t y1)
{
    int16_t steep = abs(y1 - y0) > abs(x1 - x0);
    if (steep) {
        _swap_int16_t(x0, y0);
        _swap_int16_t(x1, y1);
    }

    if (x0 > x1) {
        _swap_int16_t(x0, x1);
        _swap_int16_t(y0, y1);
    }

    int16_t dx, dy;
    dx = x1 - x0;
    dy = abs(y1 - y0);

    int16_t err = dx / 2;
    int16_t ystep;

    if (y0 < y1) {
        ystep = 1;
    } else {
        ystep = -1;
    }

    for (; x0<=x1; x0++) {
        if (steep) {
            display_draw_col_pixel(display, y0, x0);
        } else {
            display_draw_col_pixel(display, x0, y0);
        }
        err -= dy;
        if (err < 0) {
            y0 += ystep;
            err += dx;
        }
    }
}

void display_impl_draw_col_pixel(DisplayImpl* display, int16_t x, int16_t y)
{
  if (x < display->_width && x >= 0 && y < display->_height && y >= 0)
  {
    *(display->_pixels + y * display->_width + x) = 0xf0;
  }
}*/

/*
 *  Translate 1 byte pp layout to 1 bit pp layout of virtual screen buffer.
 */
uint8_t* display_impl_get_buffer(DisplayImpl* display)
{
  uint8_t tmp[1024];
  uint8_t* pixels_ptr = display->_pixels;
  for (int i = 0; i < countof(display->_buffer); ++i, pixels_ptr += 8)
  {
    tmp[i] = (*(pixels_ptr + 0) ? 0b10000000 : 0 )
           | (*(pixels_ptr + 1) ? 0b01000000 : 0 )
           | (*(pixels_ptr + 2) ? 0b00100000 : 0 )
           | (*(pixels_ptr + 3) ? 0b00010000 : 0 )
           | (*(pixels_ptr + 4) ? 0b00001000 : 0 )
           | (*(pixels_ptr + 5) ? 0b00000100 : 0 )
           | (*(pixels_ptr + 6) ? 0b00000010 : 0 )
           | (*(pixels_ptr + 7) ? 0b00000001 : 0 );
  }

  util_horiz_pixel_buf_to_vert(tmp, display->_buffer);

  return display->_buffer;
}

/*
 * Translate 1 bit pp layout to 1 byte pp layout of actual screen buffer.
 */
void display_impl_release_buffer(DisplayImpl* display)
{
  uint8_t* pixels_ptr = display->_pixels;
  uint8_t tmp[1024];

  util_vert_pixel_buf_to_horiz(display->_buffer, tmp);

  for (int i = 0; i < countof(tmp); ++i)
  {
    uint8_t byte = tmp[i];
    for (int j = 0; j < 8; ++j, ++pixels_ptr)
    {
      *pixels_ptr = (byte & (1 << (7-j)) ? 0xff : 0x00);
    }
  }
}
