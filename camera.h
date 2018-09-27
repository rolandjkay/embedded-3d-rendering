#ifndef _CAMERA_H
#define _CAMERA_H

#include "vector.h"
#include "matrix.h"

typedef struct
{
  Vector _camera_location;
  Vector _look_point;
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
const Matrix* camera_get_location_transform(Camera* camera);
const Matrix* camera_get_look_transform(Camera* camera);

#endif
