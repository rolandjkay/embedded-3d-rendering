#ifndef _SIMPLE_RENDERER

#include "vector.h"
#include "matrix.h"
#include "polygon.h"
#include "camera.h"
#include "display.h"
#include <stdlib.h>


typedef struct
{
  const Polygon* _polygons;
  size_t _num_polygons;
  Vector* _normals;  // <- We own the memory.
  float _near_plane;
  float _far_plane;
  Matrix _perspective_transform;
} SimpleRenderer;

#define SR_INIT {NULL, 0, NULL, 0, 0, 0.0, 0.0, MATRIX_ZERO};

/*
 * Initialize the renderer with an array of polygons that make up the form.
 */
void sr_init(SimpleRenderer* self, const Polygon*, size_t n,
             float near_plane, float far_plane, float near_plane_width);

void sr_render(SimpleRenderer* self, Camera* camera, Display* display);

#endif
