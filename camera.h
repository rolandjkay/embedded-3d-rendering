#ifndef _CAMERA_H
#define _CAMERA_H

#include "settings.h"
#include "fix8_matrix.h"

#ifdef INCLUDE_FLOAT_MATHS
#  include "vector.h"
#  include "matrix.h"
#endif


typedef struct
{
  int16_vector_t camera_location;
  //int16_vector_t look_point;
  fix8_vector_t look_vector;      // The direction that we're looking in (unit)
  fix8_vector_t up_vector;       // The up direction. (unit vector)
#ifdef INCLUDE_FLOAT_MATHS
  Matrix _float_camera_look_transform;
#endif
  fix8_matrix_t _camera_look_transform;
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
static inline const fix8_matrix_t* camera_get_look_transform(const Camera* self)
{
  return &self->_camera_look_transform;
}

#endif
