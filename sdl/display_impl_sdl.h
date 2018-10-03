#ifndef _SDL_DISPLAY_IMPL
#define _SDL_DISPLAY_IMPL

#include <SDL2/SDL.h>

typedef struct
{
  SDL_Window* _window;
  SDL_Renderer* _renderer;
  SDL_Surface* _screen;
  SDL_Texture * _texture;
  uint8_t _pixels[1024];
  int _width;
  int _height;
  // Virtual screen buffer in 1 bit per pixel layout.
  // - SDL doesn't support this, so we have to marshal back and forth between
  //   its 1 byte per pixel format.
  uint8_t _buffer[1024];
} DisplayImpl;

#define DISPLAY_IMPL_INIT {0}

int display_impl_init(DisplayImpl* self);
static inline int display_impl_get_width(DisplayImpl* self) { return self->_width; }
static inline int display_impl_get_height(DisplayImpl* self) { return self->_height; }
void display_impl_cls(DisplayImpl* self);
void display_impl_show(DisplayImpl* self);
void display_impl_draw_pixel(DisplayImpl* self, int16_t x, int16_t y);
uint8_t* display_impl_get_buffer(DisplayImpl* display);
void display_impl_release_buffer(DisplayImpl* display);


#endif
