#ifndef _DISPLAY_H
#define _DISPLAY_H

#include <SDL2/SDL.h>
#include "stdint.h"

typedef struct
{
  SDL_Window* _window;
  SDL_Renderer* _renderer;
  SDL_Surface* _screen;
  SDL_Texture * _texture;
  uint8_t* _pixels;
  int _width;
  int _height;
  // Virtual screen buffer in 1 bit per pixel layout.
  // - SDL doesn't support this, so we have to marshal back and forth between
  //   its 1 byte per pixel format.
  uint8_t _buffer[1024];
} Display;

typedef void UpdateFunc(Display*, uint32_t clock);

/*
 * Ctor and dtor
 */
Display* display_new();
void free_display(Display* self);

static inline int display_get_width(Display* display) { return display->_width; }
static inline int display_get_height(Display* display) { return display->_height; }

/*
 * Errors
 */
const char* get_display_error();

/*
 * Event loop
 */
void run_event_loop(Display* display, UpdateFunc* update_func);

/*
 * Drawing
 */
void display_cls(Display* self);
void display_draw_line(Display* self, int16_t x0, int16_t y0, int16_t x1, int16_t y1);
void display_draw_col_line(Display* self, int16_t x0, int16_t y0, int16_t x1, int16_t y1);
void display_draw_pixel(Display* self, int16_t x, int16_t y);
void display_draw_col_pixel(Display* self, int16_t x, int16_t y);
uint8_t display_pixel_colour(Display* self, int16_t x, int16_t y);

/*
 * Access to virtual buffer
 */
uint8_t* display_get_buffer(Display* display);

void display_release_buffer(Display* display);

#endif
