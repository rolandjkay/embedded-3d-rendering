#ifndef _CAMERA_H
#define _CAMERA_H

#include "vector.h"
#include "matrix.h"

typedef struct
{
  Vector _camera_location;
  Vector _look_point;
  Vector _up_vector;
  Vector _look_vector; // = look_point - camera_location
  Matrix _camera_location_transform;
  Matrix _camera_look_transform;
} Camera;

#define CAMERA_INIT {0}

/*
 * Ctor
 */
Camera* camera_new();


/*
 * In-situ initialization.
 */
void camera_init(Camera* self, const Vector* location, const Vector* look_point);

/*
 * Recalculate the transform matrices after changing location or look_point.
 */
void camera_calc_transforms(Camera* self);

/*
 * Accessors
 */
void camera_set_location(Camera* self, float x, float y, float z);
void camera_set_look_point(Camera* self, float x, float y, float z);
void camera_set_up_vector(Camera* self, float x, float y, float z);

static inline const Matrix* camera_get_location_transform(Camera* self)
{
  return &self->_camera_location_transform;
}

static inline const Matrix* camera_get_look_transform(Camera* self)
{
  return &self->_camera_look_transform;
}

static inline const Vector* camera_get_look_vector(Camera* self)
{
  return &self->_look_vector;
}

#endif
