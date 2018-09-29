#ifndef _SIMPLE_RENDERER

#include "vector.h"
#include "matrix.h"
#include "polygon.h"
#include "camera.h"
#include "display.h"
#include "3d_model.h"
#include <stdlib.h>


typedef struct
{
  const Scene* _scene;
  float _near_plane;
  float _far_plane;
  Matrix _perspective_transform;
} SimpleRenderer;

#define SR_INIT {NULL, 0, NULL, 0, 0, 0.0, 0.0, MATRIX_ZERO};

/*
 * Initialize the renderer with an array of polygons that make up the form.
 */
void sr_init(SimpleRenderer* self,
             const Scene* scene,
             float near_plane,
             float far_plane,
             float near_plane_width);

void sr_render_scene(SimpleRenderer* self, Camera* camera, Display* display);
void sr_render_object(SimpleRenderer* self,
                      const Object* object,
                      const Matrix* rotation_matrix,
                      Camera* camera,
                      Display* display);

#endif
