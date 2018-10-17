#include "fix8_matrix.h"

#include "fix8_vector.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

void fix8_matrix_tf_camera_look(fix8_matrix_t* dst,
                                const int16_vector_t* camera_location,
                                const fix8_vector_t* up_vector,
                                const fix8_vector_t* look_vector)
{
  // Make 'a' and 'b' mutually orthogonal unit vectors which are orthogonal
  // to the look vector.
  fix8_vector_cross_product(&dst->a, up_vector, look_vector);
  fix8_vector_cross_product(&dst->b, look_vector, &dst->a);
  vector_copy(&dst->c, look_vector);
}


void fix8_matrix_identity(fix8_matrix_t* dst)
{
  memset(dst, 0, sizeof(fix8_matrix_t));
  dst->a.x = 1.0;
  dst->b.y = 1.0;
  dst->c.z = 1.0;
}

// Angles need to be in fractions of pi.
void fix8_matrix_tf_rotation(fix8_matrix_t* dst, fix8_t xangle, fix8_t yangle, fix8_t zangle)
{
  // Because we cannot represent 1 accuratly, if we have expressions like
  // cos(y)sin(z) the result won't be equal to sin(z) when y is zero, because
  // cos(y) = 127/128 and not 1.0 Therefore, we simplify the matrix in the case
  // that any angles are zero. This also reduces computation time, but at the
  // expence of more code in flash.
  if (yangle == 0)
  {
    if (xangle == 0)
    {
      register fix8_t cz = fix8_cos(zangle);
      register fix8_t sz = fix8_sin(zangle);
      dst->a.x = cz;
      dst->a.y = -sz;
      dst->a.z = 0;

      dst->b.x = sz;
      dst->b.y = cz;
      dst->b.z = 0;

      dst->c.x = 0;
      dst->c.y = 0;
      dst->c.z = 127;
    }
    else
    {
      if (zangle == 0)
      {
        register fix8_t cx = fix8_cos(xangle);
        register fix8_t sx = fix8_sin(xangle);
        dst->a.x = 127;
        dst->a.y = 0;
        dst->a.z = 0;

        dst->b.x = 0;
        dst->b.y = cx;
        dst->b.z = -sx;

        dst->c.x = 0;
        dst->c.y = sx;
        dst->c.z = cx;
      }
      else
      {
        register fix8_t cx = fix8_cos(xangle);
        register fix8_t sx = fix8_sin(xangle);
        register fix8_t cz = fix8_cos(zangle);
        register fix8_t sz = fix8_sin(zangle);

        dst->a.x = cz;
        dst->a.y = -sz;
        dst->a.z = 0;

        dst->b.x = fix8_mul(cx, sz);
        dst->b.y = fix8_mul(cx, cz);
        dst->b.z = -sx;

        dst->c.x = fix8_mul(sx, sz);
        dst->c.y = fix8_mul(sx, cz);
        dst->c.z = cx;
      }
    }
  }
  else
  {
    if (zangle == 0)
    {
      if (xangle == 0)
      {
        register fix8_t cy = fix8_cos(yangle);
        register fix8_t sy = fix8_sin(yangle);

        dst->a.x = cy;
        dst->a.y = -0;
        dst->a.z = sy;

        dst->b.x = 0;
        dst->b.y = 127;
        dst->b.z = 0;

        dst->c.x = -sy;
        dst->c.y = 0;
        dst->c.z = cy;
      }
      else
      {
        register fix8_t cx = fix8_cos(xangle);
        register fix8_t sx = fix8_sin(xangle);
        register fix8_t cy = fix8_cos(yangle);
        register fix8_t sy = fix8_sin(yangle);

        dst->a.x = cy;
        dst->a.y = -0;
        dst->a.z = sy;

        dst->b.x = fix8_mul(sx, sy);
        dst->b.y = cx;
        dst->b.z = -fix8_mul(sx, cy);

        dst->c.x = -fix8_mul(cx, sy);
        dst->c.y = sx;
        dst->c.z = fix8_mul(cx, cy);
      }
    }
    else
    {
      if (xangle == 0)
      {
        register fix8_t cy = fix8_cos(yangle);
        register fix8_t sy = fix8_sin(yangle);
        register fix8_t cz = fix8_cos(zangle);
        register fix8_t sz = fix8_sin(zangle);

        dst->a.x = fix8_mul(cy, cz);
        dst->a.y = -fix8_mul(cy, sz);
        dst->a.z = sy;

        dst->b.x = sz;
        dst->b.y = cz;
        dst->b.z = 0;

        dst->c.x = -fix8_mul(sy, cz);
        dst->c.y = fix8_mul(sy, sz);
        dst->c.z = cy;
      }
      else
      {
        register fix8_t cx = fix8_cos(xangle);
        register fix8_t sx = fix8_sin(xangle);
        register fix8_t cy = fix8_cos(yangle);
        register fix8_t sy = fix8_sin(yangle);
        register fix8_t cz = fix8_cos(zangle);
        register fix8_t sz = fix8_sin(zangle);

        dst->a.x = fix8_mul(cy, cz);
        dst->a.y = -fix8_mul(cy, sz);
        dst->a.z = sy;

        dst->b.x = fix8_mul(fix8_mul(sx, sy), cz)
                 + fix8_mul(cx, sz);
        dst->b.y = -fix8_mul(fix8_mul(sx, sy), sz)
                 + fix8_mul(cx, cz);
        dst->b.z = -fix8_mul(sx, cy);

        dst->c.x = -fix8_mul(cx, fix8_mul(sy, cz))
                 + fix8_mul(sx, sz);
        dst->c.y = fix8_mul(cx, fix8_mul(sy, sz)) + fix8_mul(sx, cz);
        dst->c.z = fix8_mul(cx, cy);
      }
    }
  }
}

#ifndef __AVR
void fix8_matrix_to_log(const fix8_matrix_t* m)
{
  printf("|%f,%f,%f|\n|%f,%f,%f|\n|%f,%f,%f|\n",
          FIX_TO_FLT(m->a.x),
          FIX_TO_FLT(m->a.y),
          FIX_TO_FLT(m->a.z),
          FIX_TO_FLT(m->b.x),
          FIX_TO_FLT(m->b.y),
          FIX_TO_FLT(m->b.z),
          FIX_TO_FLT(m->c.x),
          FIX_TO_FLT(m->c.y),
          FIX_TO_FLT(m->c.z));
}
#else
  #include "avr/atmega328p/usart.h"

void fix8_matrix_to_log(const fix8_matrix_t* m)
{
  char buf[4];
  usart_transmit('|');
  usart_write_string(itoa(m->a.x, buf, 10));usart_transmit('|');
  usart_write_string(itoa(m->a.y, buf, 10));usart_transmit('|');
  usart_write_string(itoa(m->a.z, buf, 10));usart_transmit('|');
  usart_transmit('\n');usart_transmit('|');
  usart_write_string(itoa(m->b.x, buf, 10));usart_transmit('|');
  usart_write_string(itoa(m->b.y, buf, 10));usart_transmit('|');
  usart_write_string(itoa(m->b.z, buf, 10));usart_transmit('|');
  usart_transmit('\n');usart_transmit('|');
  usart_write_string(itoa(m->c.x, buf, 10));usart_transmit('|');
  usart_write_string(itoa(m->c.y, buf, 10));usart_transmit('|');
  usart_write_string(itoa(m->c.z, buf, 10));usart_transmit('|');
  usart_transmit('\n');
}

#endif
