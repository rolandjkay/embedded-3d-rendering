/*
 * Animation of a user typing a string.
 */
#ifndef _TYPED_STRING
#define _TYPED_STRING

#include "stdint.h"
#include "../display.h"


typedef struct
{
  const char* _str;
  size_t _strlen;
  size_t _ms_per_char;
  size_t _column;
  size_t _row;
} TypedString;

void typed_string_init(TypedString* self,
                       const char* str,
                       size_t ms_per_char,
                       size_t column,
                       size_t row);
void typed_string_render(TypedString* self, Display* display, uint32_t clock);

#endif
