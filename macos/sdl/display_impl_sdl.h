#ifndef _SDL_DISPLAY_IMPL
#define _SDL_DISPLAY_IMPL

#include <SDL2/SDL.h>

// Size in bytes of the backing buffer for the SDL display (1 byte per pixel)
// and the "virtual" buffer, whih simulates the layout of the SSD1306 chip.
static const size_t SDL_SCREEN_WIDTH = 128;
static const size_t SDL_SCREEN_HEIGHT = 64;
static const size_t SDL_SCREEN_BUFFER_SIZE = SDL_SCREEN_WIDTH*SDL_SCREEN_HEIGHT;
static const size_t SDL_VIRTUAL_SCREEN_BUFFER_SIZE = SDL_SCREEN_WIDTH*SDL_SCREEN_HEIGHT/8;

typedef struct
{
  SDL_Window* _window;
  SDL_Renderer* _renderer;
  SDL_Surface* _screen;
  SDL_Texture * _texture;
  uint8_t _pixels[SDL_SCREEN_BUFFER_SIZE];
  // Virtual screen buffer in 1 bit per pixel layout.
  // - SDL doesn't support this, so we have to marshal back and forth between
  //   its 1 byte per pixel format.
  uint8_t _buffer[SDL_VIRTUAL_SCREEN_BUFFER_SIZE];
} DisplayImpl;

#define DISPLAY_IMPL_INIT {0}

int display_impl_init(DisplayImpl* self);
static inline int display_impl_get_width(DisplayImpl* self) { return SDL_SCREEN_WIDTH; }
static inline int display_impl_get_height(DisplayImpl* self) { return SDL_SCREEN_HEIGHT; }
void display_impl_cls(DisplayImpl* self);
void display_impl_show(DisplayImpl* self);
void display_impl_draw_pixel(DisplayImpl* self, int8_t x, int8_t y);
void display_impl_draw_col_pixel(DisplayImpl* self, int8_t x, int8_t y);
uint8_t* display_impl_get_buffer(DisplayImpl* display);
void display_impl_release_buffer(DisplayImpl* display);


#endif
