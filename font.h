#ifndef _FONT_H
#define _FONT_H


/*
 * These versions write strings at given graphics coordinates
 */
void font_write_char(Display* display, char c, size_t x, size_t y);
void font_write_string(Display* display, const char* str, size_t x, size_t y);

/*
 * These versions write strings at given text coordinates
 */
static inline void font_write_char_at_text_pos(Display* display,
                                               char c,
                                               size_t column,
                                               size_t row)
{
  font_write_char(display, c, column << 3, row << 3);
}
static inline void font_write_string_at_text_pos(Display* display,
                                                 const char* str,
                                                 size_t column,
                                                 size_t row)
{
  font_write_string(display, str, column << 3, row << 3);
}

#endif
