/*
 * Implementation of display for the SSD1306 screen.
 */
#ifndef _DISPLAY_H
#define _DISPLAY_H

typedef struct
{
  uint8_t _screen_buffer[1024]
} SSD1306DisplayImplt;

#define DISPLAY_INIT {0}

int display_init(Display* self);

static inline int display_get_width(Display* display) { return 128; }
static inline int display_get_height(Display* display) { return 64; }

/*
 * Drawing
 */
void display_cls(Display* self);
void display_draw_line(Display* self, int16_t x0, int16_t y0, int16_t x1, int16_t y1);
void display_draw_pixel(Display* self, int16_t x, int16_t y);
uint8_t display_pixel_colour(Display* self, int16_t x, int16_t y);

/*
 * Access to screen buffer
 */
static inline uint8_t* display_get_buffer(Display* self)
{
  return &self->_screen_buffer;
}

static inline void display_release_buffer(Display* self) {}

#endif
