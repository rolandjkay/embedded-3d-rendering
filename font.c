#include <stdint.h>
#include "display.h"
#include "bitmaps_ssd1306/bbc_micro_font.h"

#define CHAR_WIDTH 8
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

static void _font_write_char_to_buffer(uint8_t* screen_buffer, char c, size_t x, size_t y)
{
  const uint8_t* char_data = &bbc_micro_font[(c - '!') << 3];
  // Offset to start of strip spanning screen and one byte high in which
  // the character lies = (y / 8) * 128  <- Note integer division.
  size_t block_start = (y >> 3) << 7;
  // Shift to place char at correct y in block; i.e. y % 8
  size_t shift = y & 0b00000111;
  uint8_t mask = 0b11111111 >> (8 - shift);

  // Don't draw if it would go off the screen.
  if (x > SCREEN_WIDTH - 8 || y > SCREEN_HEIGHT - 8) {
    return;
  }

  for (size_t column_index = 0; column_index < CHAR_WIDTH; ++column_index)
  {
    screen_buffer[block_start + x + column_index] &= mask;
    screen_buffer[block_start + x + column_index] |= char_data[column_index] << shift;
  }

  // There might be a second block of data.
  if (shift != 0)
  {
    shift = 8 - shift;
    mask = ~mask;
    block_start += SCREEN_WIDTH;
    for (size_t column_index = 0; column_index < CHAR_WIDTH; ++column_index)
    {
      screen_buffer[block_start + x + column_index] &= mask;
      screen_buffer[block_start + x + column_index] |= char_data[column_index] >> shift;
    }
  }
}

void font_write_char(Display* display, char c, size_t x, size_t y)
{
  uint8_t* screen_buffer = display_get_buffer(display);

  _font_write_char_to_buffer(screen_buffer, c, x, y);

  display_release_buffer(display);
}

void font_write_string(Display* display, const char* str, size_t n, size_t x, size_t y)
{
  uint8_t* screen_buffer = display_get_buffer(display);

  for (size_t char_index = 0; str[char_index] && char_index < n; ++char_index)
  {
    _font_write_char_to_buffer(screen_buffer, str[char_index],
                               x + (char_index<<3), y);
  }

  display_release_buffer(display);
}
