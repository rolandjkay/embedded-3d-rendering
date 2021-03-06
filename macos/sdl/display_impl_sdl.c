#include "display_impl_sdl.h"
#include "../../defs.h"
#include "../../errors.h"
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
  memset(self->_pixels, 0, sizeof(self->_pixels));
  memset(self->_buffer, 0, sizeof(self->_buffer));

  SDL_CreateWindowAndRenderer(SDL_SCREEN_WIDTH,
                              SDL_SCREEN_HEIGHT,
                              SDL_WINDOW_SHOWN,
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
                                     SDL_SCREEN_WIDTH,
                                     SDL_SCREEN_HEIGHT);
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
  memset(self->_pixels, 0, SDL_SCREEN_BUFFER_SIZE);
  SDL_UpdateTexture(self->_texture, NULL, self->_pixels, SDL_SCREEN_WIDTH);
}

void display_impl_show(DisplayImpl* self)
{
  SDL_UpdateTexture(self->_texture, NULL,
                    self->_pixels,
                    SDL_SCREEN_WIDTH);
  SDL_RenderClear(self->_renderer);
  SDL_RenderCopy(self->_renderer, self->_texture, NULL, NULL);
  SDL_RenderPresent(self->_renderer);
}


static uint8_t display_pixel_colour(DisplayImpl* display, int8_t x, int8_t y)
{
  return 0xff;
}

void display_impl_draw_pixel(DisplayImpl* display, int8_t x, int8_t y)
{
  // We adjust x & y so that the on-screen coordinates range from
  // (-64,-32) -> (+64,+32), with (0,0) in the centre. We do this because
  // having bands of valid, but invisible coordinates around the screen makes
  // is easier to draw lines whose vertices lie slightly beyond the screen.
  //
  // We use signed 8 bit integers for screen coordinates, so if we made (0,0)
  // the top-left corner then the far right would be 127, which is already the
  // maximum value.
  x += (SDL_SCREEN_WIDTH >> 1);
  y += (SDL_SCREEN_HEIGHT >> 1);

  if (x < SDL_SCREEN_WIDTH && x >= 0 && y < SDL_SCREEN_HEIGHT && y >= 0)
  {
    *(display->_pixels + y * SDL_SCREEN_WIDTH + x) = display_pixel_colour(display, x, y);
  }
}

void display_impl_draw_col_pixel(DisplayImpl* display,
                                 int8_t x, int8_t y,
                                 uint8_t colour)
{
  x += (SDL_SCREEN_WIDTH >> 1);
  y += (SDL_SCREEN_HEIGHT >> 1);

  if (x < SDL_SCREEN_WIDTH && x >= 0 && y < SDL_SCREEN_HEIGHT && y >= 0)
  {
    *(display->_pixels + y * SDL_SCREEN_WIDTH + x) = colour;
  }
}

/*
 * Translate 1 byte pp horizontal layout to 1 bit pp vertical layout of
 * virtual screen buffer.
 */
static void horiz_1byte_to_vert_1bit(const uint8_t input[SDL_SCREEN_BUFFER_SIZE],
                                     uint8_t output[SDL_VIRTUAL_SCREEN_BUFFER_SIZE])
{
  // Block is 8 lines across the screen.
  static const size_t BLOCK_SIZE = SDL_SCREEN_WIDTH*8;

  // The SSD1306 screen is made up of eight horizontal blocks each of which
  // is 8 pixels high.
  uint8_t* out_ptr = &output[0];
  for (size_t block_start = 0 ;
       block_start < SDL_SCREEN_BUFFER_SIZE;
       block_start += BLOCK_SIZE)
  {
    for (size_t column_index = 0; column_index < SDL_SCREEN_WIDTH; ++column_index, ++out_ptr)
    {
      *out_ptr = 0;
      // Highest byte (on the screen) is LSB
      for (size_t pixel_index = 0; pixel_index < 8; ++pixel_index)
      {
        uint8_t pixel_value = input[block_start + pixel_index * SDL_SCREEN_WIDTH + column_index];
        *out_ptr |= pixel_value ? (1<<pixel_index) : 0;
      }
    }
  }
}

/*
 * Inverse of above
 */
static void vert_1bit_to_horiz_1byte(const uint8_t input[1024],
                                     uint8_t output[1024*8])
{
  // NB: Not x8 this time, as 1 bit pp.
  static const size_t BLOCK_SIZE = SDL_SCREEN_WIDTH;
  uint8_t* out_ptr = &output[0];

  for (size_t block_start = 0 ; block_start < 1024; block_start += BLOCK_SIZE)
  {
    // Select LSB (1) first, then 2, then 4...
    for (uint8_t mask=1; mask != 0; mask <<= 1)
    {
      for (size_t column_index = 0; column_index < BLOCK_SIZE; ++column_index, ++out_ptr)
      {
        *out_ptr = (input[block_start + column_index] & mask) ? 0xff : 0;
      }
    }
  }
}

/*
 *  Translate 1 byte pp layout to 1 bit pp layout of virtual screen buffer.
 */
uint8_t* display_impl_get_buffer(DisplayImpl* display)
{
  horiz_1byte_to_vert_1bit(display->_pixels, display->_buffer);

  return display->_buffer;
}

/*
 * Translate 1 bit pp layout to 1 byte pp layout of actual screen buffer.
 */
void display_impl_release_buffer(DisplayImpl* display)
{
  vert_1bit_to_horiz_1byte(display->_buffer, display->_pixels);
}
