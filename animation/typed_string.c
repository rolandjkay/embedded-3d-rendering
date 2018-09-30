#include "typed_string.h"
#include "../font.h"
#include "../defs.h"

void typed_string_init(TypedString* self,
                       const char* str,
                       size_t ms_per_char,
                       size_t column,
                       size_t row)
{
  self->_str = str;
  self->_strlen = strlen(str);
  self->_ms_per_char = ms_per_char;
  self->_column = column;
  self->_row = row;
}

void typed_string_render(TypedString* self, Display* display, uint32_t clock)
{
  // Display clock / ms-per-char characters.
  font_write_string_at_text_pos(display,
                                self->_str,
                                clock / self->_ms_per_char,
                                0,0);

  if (clock % 1000 < 500)
  {
    font_write_char_at_text_pos(display, '_',
                                MIN(self->_strlen, clock / self->_ms_per_char),
                                0);
  }
}
