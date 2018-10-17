/*
 * A matrix implementation with 8 bit 1.7 format signed fixed-point maths
 */
#ifndef _FIX8_MATRIX_H
#define _FIX8_MATRIX_H

#include "fixed_point.h"
#include "fix8_vector.h"

/*
 * 3x3 8 bit fixed-point matrix
 */
typedef struct
{
  // The rows in the matrix.
  fix8_vector_t a, b, c;
} fix8_matrix_t;

static inline void fix8_matrix_left_multiply_vector(const fix8_matrix_t* m,
                                                    fix8_vector_t* v)
{
  fix8_vector_t t = *v;
  v->x = fix8_mul(m->a.x, t.x) + fix8_mul(m->a.y, t.y) + fix8_mul(m->a.z, t.z);
  v->y = fix8_mul(m->b.x, t.x) + fix8_mul(m->b.y, t.y) + fix8_mul(m->b.z, t.z);
  v->z = fix8_mul(m->c.x, t.x) + fix8_mul(m->c.y, t.y) + fix8_mul(m->c.z, t.z);
}

static inline void fix8_matrix_left_multiply_int16_vector(const fix8_matrix_t* m,
                                                          int16_vector_t* v)
{
  int16_vector_t t = *v;
  v->x = fix8_x_int16_mul(m->a.x, t.x) + fix8_x_int16_mul(m->a.y, t.y) + fix8_x_int16_mul(m->a.z, t.z);
  v->y = fix8_x_int16_mul(m->b.x, t.x) + fix8_x_int16_mul(m->b.y, t.y) + fix8_x_int16_mul(m->b.z, t.z);
  v->z = fix8_x_int16_mul(m->c.x, t.x) + fix8_x_int16_mul(m->c.y, t.y) + fix8_x_int16_mul(m->c.z, t.z);
}

void fix8_matrix_tf_rotation(fix8_matrix_t* dst, fix8_t xangle, fix8_t yangle, fix8_t zangle);
void fix8_matrix_tf_camera_look(fix8_matrix_t* dst,
                                const int16_vector_t* camera_location,
                                const fix8_vector_t* up_vector,
                                const fix8_vector_t* look_point);

void fix8_matrix_to_log(const fix8_matrix_t* m);

void fix8_matrix_identity(fix8_matrix_t* dst);


#endif
