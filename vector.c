#include "vector.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#ifdef __AVR
# include <avr/pgmspace.h>
#endif

/*Vector* vector_new(float x, float y, float z, float w)
{
  Vector* v = malloc(sizeof(Vector));

  v->x = x;
  v->y = y;
  v->z = z;
  v->w = w;

  return v;
} */

void vector_init(Vector* self, float x, float y, float z, float w)
{
  self->x = x;
  self->y = y;
  self->z = z;
  self->w = w;
}

void vector_copy_is(Vector* dst, const Vector* src)
{
  dst->x = src->x;
  dst->y = src->y;
  dst->z = src->z;
  dst->w = src->w;
}

/*Vector* vector_scale(const Vector* vector, float factor)
{
  return vector_new(factor * vector->x,
                    factor * vector->y,
                    factor * vector->z,
                    factor * vector->w);
                    return NULL;
}*/

void vector_scale_is(Vector* vector, float factor)
{
  vector->x = factor * vector->x;
  vector->y = factor * vector->y;
  vector->z = factor * vector->z;
  vector->w = factor * vector->w;
}

/*
 * Normalize a vector (in situ) by dividing by its magnitude.
 */
void vector_normalize_is(Vector* vector)
{
  float magnitude = sqrtf(vector->x * vector->x +
                          vector->y * vector->y +
                          vector->z * vector->z);

  vector->x = vector->x / magnitude;
  vector->y = vector->y / magnitude;
  vector->z = vector->z / magnitude;
}

void vector_cross_product(Vector* dst, const Vector* u, const Vector* v)
{
  // minus sign added because we're in a left-hand coordinate system.
  dst->x = -(u->y * v->z - u->z * v->y);
  dst->y = -(u->z * v->x - u->x * v->z);
  dst->z = -(u->x * v->y - u->y * v->x);
  dst->w = 1;
}

float vector_dot_product(const Vector* u, const Vector* v)
{
  return u->x * v->x  +  u->y * v->y  +  u->z * v->z;
}


void vector_subtract(Vector* dst, const Vector* u, const Vector* v)
{
  dst->x = u->x - v->x;
  dst->y = u->y - v->y;
  dst->z = u->z - v->z;
  dst->w = u->w - v->w;
}

#ifndef __AVR

void vector_to_log(Vector* self)
{
  static char buf[256]
  snprintf(str, sizeof(buf), PSTR("[%f, %f, %f, %f]"), self->x, self->y, self->z, self->w);
  return buf;
}

#else
#  include "avr/atmega328p/usart.h"

void vector_to_log(Vector* self)
{
  char buf[10];
  usart_transmit('|');
  usart_write_string(dtostrf(self->x, 2, 2, buf)); usart_transmit('|');
  usart_write_string(dtostrf(self->y, 2, 2, buf)); usart_transmit('|');
  usart_write_string(dtostrf(self->z, 2, 2, buf)); usart_transmit('|');
  usart_write_string(dtostrf(self->w, 2, 2, buf)); usart_transmit('|');
  usart_transmit('\n');
}

#endif
