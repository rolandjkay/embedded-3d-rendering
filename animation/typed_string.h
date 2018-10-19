/*
 * Animation of a user typing a string.
 */
#ifndef _TYPED_STRING
#define _TYPED_STRING

#include "stdint.h"
#include "../display.h"
#include "../defs.h"

typedef struct
{
  pgm_ptr_t _str;
  size_t _strlen;
  size_t _log_ms_per_char;
  size_t _column;
  size_t _row;
} TypedString;

void typed_string_init(TypedString* self,
                       pgm_ptr_t str,
                       size_t log_ms_per_char,
                       size_t column,
                       size_t row);
void typed_string_render(TypedString* self, Display* display, uint32_t clock);

#endif
