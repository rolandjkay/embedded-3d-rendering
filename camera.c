#include <stdlib.h>

#include "camera.h"
#include "vector.h"

/*
 * In-situ initialization.
 */
/*void camera_init(Camera* self,
                 const int16_vector_t* location,
                 const int16_vector_t* look_point)
                 const int16_vector_t* look_point)
{
  self->camera_location = *location;
  self->look_point = *look_point;
}*/

/*
 * Calculate the transformations
 */
void camera_calc_transforms(Camera* self)
{
  matrix_tf_camera_look(&self->_camera_look_transform,
                        &self->camera_location,
                        &self->up_vector,
                        &self->look_point);
  fix8_matrix_tf_camera_look(&self->_fx_camera_look_transform,
                             &self->camera_location,
                             &self->up_vector,
                             &self->look_point);
  vector_subtract(&self->_look_vector,
                  &self->look_point,
                  &self->camera_location);
}

/*
 * Accessors
 */
//void camera_set_location(Camera* self, float x, float y, float z)
//{
//  vector_init(&self->_camera_location, x, y, z);
//}

//void camera_set_look_point(Camera* self, float x, float y, float z)
//{
//  vector_init(&self->_look_point, x, y, z);
//}

//void camera_set_up_vector(Camera* self, float x, float y, float z)
//{
//  vector_init(&self->_up_vector, x, y, z);
//}
