#include "display.h"
#include "defs.h"
#include <sys/time.h>

enum Error {
  DISPLAY_ERROR_OK,
  DISPLAY_ERROR_SDL,
  DISPLAY_ERROR_OOM,
} error;

const char* get_display_error()
{
  switch (error) {
    case DISPLAY_ERROR_OK: return "No display error";
    case DISPLAY_ERROR_SDL: return SDL_GetError();
    case DISPLAY_ERROR_OOM: return "Memory allocation failure";
    default:
      return "Unknown display error";
  }
}

int init_display() {
  if (SDL_Init( SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0 )
  {
    error = DISPLAY_ERROR_SDL;
    return -1;
  }
  else
  {
    return 0;
  }
}

Display* display_new(Uint32 width, Uint32 height)
{
  Display* display = malloc(sizeof(Display));
  if (display == NULL) {
    error = DISPLAY_ERROR_OOM;
    return NULL;
  }

  display->_window = NULL;
  display->_renderer = NULL;
  display->_screen = NULL;
  display->_texture = NULL;
  display->_pixels = NULL;
  display->_width = width;
  display->_height = height;
  memset(display->_buffer, 0, sizeof(display->_buffer));

  SDL_CreateWindowAndRenderer(width, height, SDL_WINDOW_SHOWN,
                              &display->_window, &display->_renderer);

  if (!display->_window || !display->_renderer) {
    error = DISPLAY_ERROR_SDL;
    return NULL;
  }

  /*
   * We're not allow to create 1 bit per pixel (indexed, or otherwise) textures.
   * So, we use RGB332, which is at least one byte per pixel, and write 0xff
   * for white and 0x00 for black.
   */
  display->_texture = SDL_CreateTexture(display->_renderer,
                                       SDL_PIXELFORMAT_RGB332,
                                       SDL_TEXTUREACCESS_STATIC,
                                       width, height);
  if (!display->_texture) {
    error = DISPLAY_ERROR_SDL;
    return NULL;
  }

  /*
   * Allocate display buffer
   */
  display->_pixels = malloc(width * height);
  if (display->_pixels == NULL) {
    error = DISPLAY_ERROR_OOM;
    return NULL;
  }

  // Blank display
  display_cls(display);

  return display;
}

void free_display(Display* display)
{
  if (display->_pixels) {
    free(display->_pixels);
  }
  free(display);
}

// Timer callback function which diverts event back to the main thread
/* with the same code as before: */
static Uint32 timer_callbackfunc(Uint32 interval, void *param)
{
  SDL_Event event;
  SDL_UserEvent userevent;

  /* In this example, our callback pushes a function
  into the queue, and causes our callback to be called again at the
  same interval: */

  userevent.type = SDL_USEREVENT;
  userevent.code = 0;

  event.type = SDL_USEREVENT;
  event.user = userevent;

  SDL_PushEvent(&event);
  return(interval);
}

void run_event_loop(Display* display, UpdateFunc* update_func)
{
  int quit = 0;
  uint32_t clock = 0; // We pass this timestamp to the update function.
  SDL_Event event;

  struct timeval start_tv, tv;
  int e = gettimeofday(&start_tv, NULL);

  if (e != 0)
  {
    fprintf(stderr, "Couldn't get time of day.");
    return;
  }


  // Start the refresh timer
  Uint32 delay = 100;  /* To round it down to the nearest 10 ms */
  SDL_TimerID refresh_timer_id = SDL_AddTimer(delay, timer_callbackfunc, NULL);

  while (!quit)
  {
    SDL_WaitEvent(&event);
    switch (event.type)
    {
     case SDL_QUIT:
       quit = 1;
       break;

     // Our redraw timer fired
     case SDL_USEREVENT:
       // Call update func
       if (update_func)
       {
         e = gettimeofday(&tv, NULL);

         if (e != 0)
         {
           fprintf(stderr, "Couldn't get time of day\n");
           clock += 100;
         }
         else
         {
           // Calculate clock in 1024th of a second.
           clock = (tv.tv_sec - start_tv.tv_sec) << 10;
           clock += (tv.tv_usec - start_tv.tv_usec) / 1024;
           printf("clock=%d\n", clock);
         }

         update_func(display, clock);

         SDL_UpdateTexture(display->_texture, NULL,
                           display->_pixels, display->_width);
         SDL_RenderClear(display->_renderer);
         SDL_RenderCopy(display->_renderer, display->_texture, NULL, NULL);
         SDL_RenderPresent(display->_renderer);
       }
       break;
    }
  }

  SDL_RemoveTimer(refresh_timer_id);
}



/*
 * Drawing
 */

#ifndef _swap_int16_t
#define _swap_int16_t(a, b) { int16_t t = a; a = b; b = t; }
#endif

void display_cls(Display* self)
{
  // Blank display
  memset(self->_pixels, 0, self->_width * self->_height);

  SDL_UpdateTexture(self->_texture, NULL, self->_pixels, self->_width);
}

/**************************************************************************/
/*!
   @brief    Write a line.  Bresenham's algorithm - thx wikpedia
    @param    x0  Start point x coordinate
    @param    y0  Start point y coordinate
    @param    x1  End point x coordinate
    @param    y1  End point y coordinate
*/
/**************************************************************************/
void display_draw_line(Display* display, int16_t x0, int16_t y0, int16_t x1, int16_t y1)
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
            display_draw_pixel(display, y0, x0);
        } else {
            display_draw_pixel(display, x0, y0);
        }
        err -= dy;
        if (err < 0) {
            y0 += ystep;
            err += dx;
        }
    }
}

void display_draw_pixel(Display* display, int16_t x, int16_t y)
{
  if (x < display->_width && x >= 0 && y < display->_height && y >= 0)
  {
    *(display->_pixels + y * display->_width + x) = display_pixel_colour(display, x, y);
  }
}

uint8_t display_pixel_colour(Display* display, int16_t x, int16_t y)
{
  return 0xff;
}


// XXX HORRIBLE COPY-PASTE
//
void display_draw_col_line(Display* display, int16_t x0, int16_t y0, int16_t x1, int16_t y1)
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

void display_draw_col_pixel(Display* display, int16_t x, int16_t y)
{
  if (x < display->_width && x >= 0 && y < display->_height && y >= 0)
  {
    *(display->_pixels + y * display->_width + x) = 0xf0;
  }
}

/*
 *  Translate 1 byte pp layout to 1 bit pp layout of virtual screen buffer.
 */
uint8_t* display_get_buffer(Display* display)
{
  uint8_t* pixels_ptr = display->_pixels;
  for (int i = 0; i < countof(display->_buffer); ++i, pixels_ptr += 8)
  {
    display->_buffer[i] = (*(pixels_ptr + 0) ? 0b10000000 : 0 )
                        | (*(pixels_ptr + 1) ? 0b01000000 : 0 )
                        | (*(pixels_ptr + 2) ? 0b00100000 : 0 )
                        | (*(pixels_ptr + 3) ? 0b00010000 : 0 )
                        | (*(pixels_ptr + 4) ? 0b00001000 : 0 )
                        | (*(pixels_ptr + 5) ? 0b00000100 : 0 )
                        | (*(pixels_ptr + 6) ? 0b00000010 : 0 )
                        | (*(pixels_ptr + 7) ? 0b00000001 : 0 );
  }

  return display->_buffer;
}

/*
 * Translate 1 bit pp layout to 1 byte pp layout of actual screen buffer.
 */
void display_release_buffer(Display* display)
{
  uint8_t* pixels_ptr = display->_pixels;
  for (int i = 0; i < countof(display->_buffer); ++i)
  {
    uint8_t byte = display->_buffer[i];
    for (int j = 0; j < 8; ++j, ++pixels_ptr)
    {
      *pixels_ptr = (byte & (1 << (7-j)) ? 0xff : 0x00);
    }
  }
}
