#include <stdlib.h>

#include "camera.h"
#include "vector.h"

/*
 * Heap allocation
 */
Camera* camera_new()
{
  return malloc(sizeof(Camera));
}

/*
 * In-situ initialization.
 */
void camera_init(Camera* self, const Vector* location, const Vector* look_point)
{
  vector_copy(&self->_camera_location, location);
  vector_copy(&self->_look_point, look_point);
}

/*
 * Calculate the transformations
 */
void camera_calc_transforms(Camera* self)
{
  matrix_tf_camera_look(&self->_camera_look_transform,
                        &self->_camera_location,
                        &self->_up_vector,
                        &self->_look_point);
  vector_subtract(&self->_look_vector,
                  &self->_look_point,
                  &self->_camera_location);
}

/*
 * Accessors
 */
void camera_set_location(Camera* self, float x, float y, float z)
{
  vector_init(&self->_camera_location, x, y, z, 1.0);
}

void camera_set_look_point(Camera* self, float x, float y, float z)
{
  vector_init(&self->_look_point, x, y, z, 1.0);
}

void camera_set_up_vector(Camera* self, float x, float y, float z)
{
  vector_init(&self->_up_vector, x, y, z, 1.0);
}
