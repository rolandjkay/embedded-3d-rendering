#include "vector.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

void vector_init(Vector* self, float x, float y, float z)
{
  self->x = x;
  self->y = y;
  self->z = z;
}

void vector_copy(Vector* dst, const Vector* src)
{
  dst->x = src->x;
  dst->y = src->y;
  dst->z = src->z;
}

void vector_scale(Vector* vector, float factor)
{
  vector->x = factor * vector->x;
  vector->y = factor * vector->y;
  vector->z = factor * vector->z;
}

/*
 * Normalize a vector (in situ) by dividing by its magnitude.
 */
void vector_normalize(Vector* vector)
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
}

#ifndef __AVR

void vector_to_log(Vector* self)
{
  printf("[%f, %f, %f]", self->x, self->y, self->z);
}

#else
#  include "avr/atmega328p/usart.h"

void vector_to_log(Vector* self)
{
  char buf[10];
  usart_transmit('[');
  usart_write_string(dtostrf(self->x, 2, 2, buf)); usart_transmit(',');
  usart_write_string(dtostrf(self->y, 2, 2, buf)); usart_transmit(',');
  usart_write_string(dtostrf(self->z, 2, 2, buf)); usart_transmit(',');
  usart_transmit('\n');
}

#endif
