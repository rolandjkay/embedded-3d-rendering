#ifndef _MATRIX_H
#define _MATRIX_H

#include "vector.h"

/*
 * 3x3 matrix
 */
typedef struct
{
  float data[9];
} Matrix;

#define MATRIX_ZERO {0};

void matrix_zero(Matrix* dst);
void matrix_identity(Matrix* dst);

/*
 * Operations
 */
void matrix_multiply(Matrix* dst, const Matrix* m1, const Matrix* m2);

void matrix_left_multiply_vector(const Matrix*, Vector* Vector);

/*
 * Display
 */

void matrix_to_log(const Matrix* m);

/*
 * Transforms
 */
void matrix_tf_translation(Matrix* dst, const Vector* vector);
void matrix_tf_rotation(Matrix* dst, float xangle, float yangle, float zangle);

void matrix_tf_change_of_basis(Matrix* dst,
                                const Vector* i,
                                const Vector* j,
                                const Vector* k);

void matrix_tf_camera_look(Matrix* dst,
                           const int16_vector_t* camera_location,
                           const fix8_vector_t* up_vector,
                           const int16_vector_t* look_point);

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
  v->x = int16_x_fix8_mul(m->a.x, t.x) + int16_x_fix8_mul(m->a.y, t.y) + int16_x_fix8_mul(m->a.z, t.z);
  v->y = int16_x_fix8_mul(m->b.x, t.x) + int16_x_fix8_mul(m->b.y, t.y) + int16_x_fix8_mul(m->b.z, t.z);
  v->z = int16_x_fix8_mul(m->c.x, t.x) + int16_x_fix8_mul(m->c.y, t.y) + int16_x_fix8_mul(m->c.z, t.z);
}

void fix8_matrix_tf_rotation(fix8_matrix_t* dst, fix8_t xangle, fix8_t yangle, fix8_t zangle);
void fix8_matrix_tf_camera_look(fix8_matrix_t* dst,
                                const int16_vector_t* camera_location,
                                const fix8_vector_t* up_vector,
                                const int16_vector_t* look_point);

void fix8_matrix_to_log(const fix8_matrix_t* m);

void fix8_matrix_identity(fix8_matrix_t* dst);


#endif
