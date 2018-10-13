#include "matrix.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>


void matrix_zero(Matrix* self)
{
  memset(self, 0, sizeof(Matrix));
}

void matrix_identity(Matrix* self)
{
  memset(self, 0, sizeof(Matrix));
  self->data[0] = 1.0;
  self->data[4] = 1.0;
  self->data[8] = 1.0;
}

/*
 * Matrix multiplication.
 */
void matrix_left_multiply_vector(const Matrix* m, Vector* Vector)
{
  float x = Vector->x, y = Vector->y, z = Vector->z;;
  Vector->x = m->data[0] * x + m->data[1] * y + m->data[2] * z;
  Vector->y = m->data[3] * x + m->data[4] * y + m->data[5] * z;
  Vector->z = m->data[6] * x + m->data[7] * y + m->data[8] * z;
}

/*
 * Operations
 */

void matrix_multiply(Matrix* dst, const Matrix* m1, const Matrix* m2)
{
  for (int i = 0; i < 3; ++i) {
    for (int j = 0; j < 3; ++j) {
        float x = 0.0;
        for (int k = 0; k < 3; ++k)
        {
          x += m1->data[3 * i + k] * m2->data[3 * k + j];
        }
        dst->data[3 * i + j] = x;
    }
  }
}

/*
 * Display
 */
#ifndef __AVR
void matrix_to_log(const Matrix* m)
{
  printf("|%f,%f,%f|\n|%f,%f,%f|\n|%f,%f,%f|",
          m->data[0],
          m->data[1],
          m->data[2],
          m->data[3],
          m->data[4],
          m->data[5],
          m->data[6],
          m->data[7],
          m->data[8]);
}
#else
  #include "avr/atmega328p/usart.h"

void matrix_to_log(const Matrix* m)
{
  char buf[10];
  usart_transmit('|');
  usart_write_string(dtostrf(m->data[0], 2, 2, buf));usart_transmit('|');
  usart_write_string(dtostrf(m->data[1], 2, 2, buf));usart_transmit('|');
  usart_write_string(dtostrf(m->data[2], 2, 2, buf));usart_transmit('|');
  usart_transmit('\n');usart_transmit('|');
  usart_write_string(dtostrf(m->data[3], 2, 2, buf));usart_transmit('|');
  usart_write_string(dtostrf(m->data[4], 2, 2, buf));usart_transmit('|');
  usart_write_string(dtostrf(m->data[5], 2, 2, buf));usart_transmit('|');
  usart_transmit('\n');usart_transmit('|');
  usart_write_string(dtostrf(m->data[6], 2, 2, buf));usart_transmit('|');
  usart_write_string(dtostrf(m->data[7], 2, 2, buf));usart_transmit('|');
  usart_write_string(dtostrf(m->data[8], 2, 2, buf));usart_transmit('|');
  usart_transmit('\n');
}

#endif

/*
 * Transforms
 */

/*
 * Rotation matrix
 */
void matrix_tf_rotation(Matrix* dst, float xangle, float yangle, float zangle)
{
  dst->data[0] = cos(yangle) * cos(zangle);
  dst->data[1] = -cos(yangle) * sin(zangle);
  dst->data[2] = sin(yangle);

  dst->data[3] = sin(xangle) * sin(yangle) * cos(zangle) + cos(xangle) * sin(zangle);
  dst->data[4] = -sin(xangle) * sin(yangle) * sin(zangle) + cos(xangle) * cos(zangle);
  dst->data[5] = -sin(xangle) * cos(yangle);

  dst->data[6] = -cos(xangle) * sin(yangle) * cos(zangle) + sin(xangle) * sin(zangle);
  dst->data[7] = cos(xangle) * sin(yangle) * sin(zangle) + sin(xangle) * cos(zangle);
  dst->data[8] = cos(xangle) * cos(yangle);
}


 void matrix_tf_change_of_basis(Matrix* dst,
                                const Vector* i,
                                const Vector* j,
                                const Vector* k)
 {
   dst->data[0] = i->x;
   dst->data[1] = i->y;
   dst->data[2] = i->z;

   dst->data[3] = j->x;
   dst->data[4] = j->y;
   dst->data[5] = j->z;

   dst->data[6] = k->x;
   dst->data[7] = k->y;
   dst->data[8] = k->z;
 }

void matrix_tf_camera_look(Matrix* dst,
                           const int16_vector_t* camera_location,
                           const fix8_vector_t* up_vector,
                           const int16_vector_t* look_point)
{
  Vector u, v, n, _up_vector;

  _up_vector.x = ((float)up_vector->x) / 64.;
  _up_vector.y = ((float)up_vector->y) / 64.;
  _up_vector.z = ((float)up_vector->z) / 64.;

  vector_subtract(&n, look_point, camera_location);
  vector_normalize(&n);
  vector_cross_product(&u, &_up_vector, &n);
  vector_cross_product(&v, &n, &u);

  matrix_tf_change_of_basis(dst, &u, &v, &n);
}

void fix8_matrix_tf_camera_look(fix8_matrix_t* dst,
                                const int16_vector_t* camera_location,
                                const fix8_vector_t* up_vector,
                                const int16_vector_t* look_point)
{
  Vector u, v, n, _up_vector;

  vector_subtract(&n, look_point, camera_location);
  vector_normalize(&n);
  vector_cross_product(&u, &_up_vector, &n);
  vector_cross_product(&v, &n, &u);

  // Now that we have three unit vectors, with float underlying, it's easier
  // to convert to fix8_t. It would be nice to avoid the floats altogether,
  // but vector normalization is a bit complicated with fixed-point. It
  // doesn't matter too much because
  //  1) The final matrix is using fix8_t, so doesn't take as much space.
  //  2) This computation only happen on camera move, which is never in the demo.
  // We could save some flash by not linking the float lib, if we got rid of
  // all float operations, but it's not a priority.
  dst->a.x = (fix8_t)(u.x*64.0f); dst->a.y = (fix8_t)(u.y*64.0f); dst->a.z = (fix8_t)(u.z*64.0f);
  dst->b.x = (fix8_t)(v.x*64.0f); dst->b.y = (fix8_t)(v.y*64.0f); dst->b.z = (fix8_t)(v.z*64.0f);
  dst->c.x = (fix8_t)(n.x*64.0f); dst->c.y = (fix8_t)(n.y*64.0f); dst->c.z = (fix8_t)(n.z*64.0f);
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
  dst->a.x = fix8_mul(fix8_cos(yangle), fix8_cos(zangle));
  dst->a.y = -fix8_mul(fix8_cos(yangle), fix8_sin(zangle));
  dst->a.z = fix8_sin(yangle);

  dst->b.x = fix8_mul(fix8_mul(fix8_sin(xangle), fix8_sin(yangle)), fix8_cos(zangle))
           + fix8_mul(fix8_cos(xangle), fix8_sin(zangle));
  dst->b.y = -fix8_mul(fix8_mul(fix8_sin(xangle), fix8_sin(yangle)), fix8_sin(zangle))
           + fix8_mul(fix8_cos(xangle), fix8_cos(zangle));
  dst->b.z = -fix8_mul(fix8_sin(xangle) ,fix8_cos(yangle));

  dst->c.x = -fix8_mul(fix8_cos(xangle), fix8_mul(fix8_sin(yangle), fix8_cos(zangle)))
           + fix8_mul(fix8_sin(xangle), fix8_sin(zangle));
  dst->c.y = fix8_mul(fix8_cos(xangle), fix8_mul(fix8_sin(yangle), fix8_sin(zangle))) + fix8_mul(fix8_sin(xangle), fix8_cos(zangle));
  dst->c.z = fix8_mul(fix8_cos(xangle), fix8_cos(yangle));
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
