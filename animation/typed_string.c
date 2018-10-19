#include "typed_string.h"
#include "../font.h"
#include "../defs.h"

#ifdef __AVR
# include <avr/pgmspace.h>
#else
# include "../macos/pgmspace.h"
#endif

void typed_string_init(TypedString* self,
                       pgm_ptr_t str,
                       size_t log_ms_per_char,
                       size_t column,
                       size_t row)
{
  self->_str = str;
  self->_strlen = strlen_P((char*)str);
  self->_log_ms_per_char = log_ms_per_char;
  self->_column = column;
  self->_row = row;
}

void typed_string_render(TypedString* self, Display* display, uint32_t clock)
{
  size_t n = clock >> self->_log_ms_per_char;

  // Display clock / ms-per-char characters.
  // XXX Easiest to assume that string is in progmem atm.
  font_write_string_at_text_pos_P(display, self->_str, n, 0,0);

  if ((clock & 1024) < 512)
  {
    font_write_char_at_text_pos(display, '_', MIN(self->_strlen, n), 0);
  }
}
