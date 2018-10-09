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
                           const Vector* camera_location,
                           const Vector* up_vector,
                           const Vector* look_point)
{
  Vector u, v, n;

  vector_subtract(&n, look_point, camera_location);
  vector_normalize(&n);
  vector_cross_product(&u, up_vector, &n);
  vector_cross_product(&v, &n, &u);

  matrix_tf_change_of_basis(dst, &u, &v, &n);
}
