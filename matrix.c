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
  self->data[5] = 1.0;
  self->data[10] = 1.0;
  self->data[15] = 1.0;
}

/*
 * In-situ matrix multiplication.
 */
void matrix_left_multiply_vector(const Matrix* m, Vector* Vector)
{
  float x = Vector->x, y = Vector->y, z = Vector->z, w = Vector->w;
  Vector->x = m->data[0] * x + m->data[1] * y + m->data[2] * z  + m->data[3] * w;
  Vector->y = m->data[4] * x + m->data[5] * y + m->data[6] * z  + m->data[7] * w;
  Vector->z = m->data[8] * x + m->data[9] * y + m->data[10] * z  + m->data[11] * w;
  Vector->w = m->data[12] * x + m->data[13] * y + m->data[14] * z  + m->data[15] * w;
}

/*
 * Operations
 */

void matrix_multiply(Matrix* dst, const Matrix* m1, const Matrix* m2)
{
  for (int i = 0; i < 4; ++i) {
    for (int j = 0; j < 4; ++j) {
        float x = 0.0;
        for (int k = 0; k < 4; ++k)
        {
          x += m1->data[4 * i + k] * m2->data[4 * k + j];
        }
        dst->data[4 * i + j] = x;
    }
  }
}

/*void matrix_combine(Matrix* dst,
                    const Matrix* m1,
                    const Matrix* m2,
                    const Matrix* m3)
{
  matrix_multiply(dst, m1, m2);
  matrix_multiply(dst, dst, m3);
}*/

/*
 * Display
 */
char* matrix_to_str(const Matrix* m)
{
  static char buf[2000];

  sprintf(buf, "|%f,%f,%f,%f|\n|%f,%f,%f,%f|\n|%f,%f,%f,%f|\n|%f,%f,%f,%f|",
          m->data[0],
          m->data[1],
          m->data[2],
          m->data[3],
          m->data[4],
          m->data[5],
          m->data[6],
          m->data[7],
          m->data[8],
          m->data[9],
          m->data[10],
          m->data[11],
          m->data[12],
          m->data[13],
          m->data[14],
          m->data[15]);

  return buf;
}


/*
 * Transforms
 */

 /*
  * Translation matrix
  */
  void matrix_tf_translation(Matrix* dst, const Vector* Vector)
  {
    memset(dst, 0, sizeof(Matrix));
    dst->data[0] = 1.0;
    dst->data[3] = Vector->x;
    dst->data[5] = 1.0;
    dst->data[7] = Vector->y;
    dst->data[10] = 1.0;
    dst->data[11] = Vector->z;
    dst->data[15] = 1.0;
  }


/*
 * Rotation matrix
 */
void matrix_tf_rotation(Matrix* dst, float xangle, float yangle, float zangle)
{
  dst->data[0] = cos(yangle) * cos(zangle);
  dst->data[1] = -cos(yangle) * sin(yangle);
  dst->data[2] = sin(yangle);
  dst->data[3] = 0.0;

  dst->data[4] = sin(xangle) * sin(yangle) * cos(zangle) + cos(xangle) * sin(zangle);
  dst->data[5] = -sin(xangle) * sin(yangle) * sin(zangle) + cos(xangle) * cos(zangle);
  dst->data[6] = -sin(xangle) * cos(yangle);
  dst->data[7] = 0.0;

  dst->data[8] = -cos(xangle) * sin(yangle) * cos(zangle) + sin(xangle) * sin(zangle);
  dst->data[9] = cos(xangle) * sin(yangle) * sin(zangle) + sin(xangle) * cos(zangle);
  dst->data[10] = cos(xangle) * cos(yangle);
  dst->data[11] = 0.0;

  dst->data[12] = 0.0;
  dst->data[13] = 0.0;
  dst->data[14] = 0.0;
  dst->data[15] = 1.0;
}


 void matrix_tf_change_of_basis(Matrix* dst,
                                const Vector* i,
                                const Vector* j,
                                const Vector* k)
 {
   dst->data[0] = i->x;
   dst->data[1] = i->y;
   dst->data[2] = i->z;
   dst->data[3] = 0.;
   dst->data[4] = j->x;
   dst->data[5] = j->y;
   dst->data[6] = j->z;
   dst->data[7] = 0.;
   dst->data[8] = k->x;
   dst->data[9] = k->y;
   dst->data[10] = k->z;
   dst->data[11] = 0.;
   dst->data[12] = 0.;
   dst->data[13] = 0.;
   dst->data[14] = 0.;
   dst->data[15] = 1.;
 }

void matrix_tf_perspective(Matrix* dst,
                           float near_plane,
                           float far_plane,
                           float viewport_width,
                           float viewport_height
                          )
{
 //  float width = -2 * near_plane * tan(field_of_view / 2.0);
 //  float height = width / r;

     dst->data[0] = 2. * near_plane / viewport_width;
     dst->data[1] = 0.;
     dst->data[2] = 0.;
     dst->data[3] = 0.;
     dst->data[4] = 0.;
     dst->data[5] = 2. * near_plane / viewport_height;
     dst->data[6] = 0.;
     dst->data[7] = 0.;
     dst->data[8] = 0.;
     dst->data[9] = 0.;
     //matrix->data[10] = -(far_plane + near_plane) / (far_plane-near_plane);
     // He has -1, but I suspect it might be +1
     dst->data[10] = (far_plane + near_plane) / (far_plane-near_plane);
     dst->data[11] = -2.0 * far_plane * near_plane / (far_plane-near_plane);
     dst->data[12] = 0.;
     dst->data[13] = 0.;
     dst->data[14] = -1.0;
     dst->data[15] = 0.;
}

void matrix_tf_camera_location(Matrix* dst, const Vector* camera_location)
{
    Vector inverse_camera_location;
    inverse_camera_location.x = camera_location->x * -1.0;
    inverse_camera_location.y = camera_location->y * -1.0;
    inverse_camera_location.z = camera_location->z * -1.0;
    inverse_camera_location.w = camera_location->w * -1.0;

    matrix_tf_translation(dst, &inverse_camera_location);
}

void matrix_tf_camera_look(Matrix* dst,
                           const Vector* camera_location,
                           const Vector* up_vector,
                           const Vector* look_point)
{
  Vector u, v, n;

  vector_subtract(&n, look_point, camera_location);
  vector_normalize_is(&n);
  vector_cross_product(&u, up_vector, &n);
  vector_cross_product(&v, &n, &u);

  matrix_tf_change_of_basis(dst, &u, &v, &n);
}
