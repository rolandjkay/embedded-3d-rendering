#ifndef _CAMERA_H
#define _CAMERA_H

#include "vector.h"
#include "matrix.h"

typedef struct
{
  int16_vector_t camera_location;
  int16_vector_t look_point;
  fix8_vector_t up_vector;
  int16_vector_t _look_vector; // = look_point - camera_location
  Matrix _camera_look_transform;
  fix8_matrix_t _fx_camera_look_transform;
} Camera;

#define CAMERA_INIT(loc, look, up) {(loc), (look), (up), {0}, {0}, {0}}

/*
 * In-situ initialization.
 */
//void camera_init(Camera* self,
//                 const int16_vector_t* location,
//                 const int16_vector_t* look_point);

/*
 * Recalculate the transform matrices after changing location or look_point.
 */
void camera_calc_transforms(Camera* self);

/*
 * Accessors
 */
//void camera_set_location(Camera* self, float x, float y, float z);
//void camera_set_look_point(Camera* self, float x, float y, float z);
//void camera_set_up_vector(Camera* self, float x, float y, float z);

/*static inline const int16_vector_t* camera_get_location(const Camera* self)
{
  return &self->_camera_location;
}
*/
static inline const Matrix* camera_get_look_transform(const Camera* self)
{
  return &self->_camera_look_transform;
}

static inline const int16_vector_t* camera_get_look_vector(const Camera* self)
{
  return &self->_look_vector;
}

#endif
