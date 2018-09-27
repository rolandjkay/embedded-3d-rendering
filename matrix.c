#include "matrix.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>



void matrix_free(Matrix* m)
{
  free(m);
}

/*
 * Translation matrix ctor
 */
 Matrix* matrix_zero()
 {
   Matrix* matrix = malloc(sizeof(Matrix));
   // XXX handle OOM

   memset(matrix, 0, sizeof(Matrix));

   return matrix;
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

 Matrix* matrix_multiply(const Matrix* m1, const Matrix* m2)
 {
   Matrix* m = matrix_zero();

   if (m == NULL)
     return NULL;

   for (int i = 0; i < 4; ++i) {
     for (int j = 0; j < 4; ++j) {
         float x = 0.0;
         for (int k = 0; k < 4; ++k)
         {
           x += m1->data[4 * i + k] * m2->data[4 * k + j];
         }
         m->data[4 * i + j] = x;
     }
   }

   return m;
 }

Matrix* matrix_combine(const Matrix* m1,
                       const Matrix* m2,
                       const Matrix* m3)
{
  Matrix* m = matrix_multiply(m1, m2);
  if (m == NULL)
    return NULL;

  Matrix* n = matrix_multiply(m, m3);
  // If 'n' is NULL, we will free 'm' & return NULL; which is what we want.

  matrix_free(m);

  return n;
}

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
  * Translation matrix ctor
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
                           const Vector* look_point)
{
  const Vector up = {0, 1, 0, 1};
  Vector u, v, n;

  vector_subtract(&n, look_point, camera_location);
  vector_normalize_is(&n);
  vector_cross_product(&u, &up, &n);
  vector_cross_product(&v, &n, &u);

  matrix_tf_change_of_basis(dst, &u, &v, &n);
}
