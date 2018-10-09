#ifndef _MATRIX_H
#define _MATRIX_H

#include "vector.h"

/*
 * 4x4 matrix
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
                           const Vector* camera_location,
                           const Vector* up_vector,
                           const Vector* look_point);

#endif
