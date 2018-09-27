#ifndef _DISPLAY_H
#define _DISPLAY_H

#include "types.h"

typedef struct _Display Display;

typedef void UpdateFunc(Display*, uint32_t clock);

/*
 * Ctor and dtor
 */
Display* display_new();
void free_display(Display* self);

int display_get_width(Display* self);
int display_get_height(Display* self);

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
void display_draw_pixel(Display* self, int16_t x, int16_t y);
uint8_t display_pixel_colour(Display* self, int16_t x, int16_t y);

#endif
