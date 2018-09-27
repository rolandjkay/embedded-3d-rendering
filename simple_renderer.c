#include <stdio.h>
#include "simple_renderer.h"


/*
 * Calculate the normal facing out of the form.
 */
void _calc_normal(Vector* normal_out, const Polygon* polygon)
{
  Vector a, b;
  vector_subtract(&a, &polygon->vertices[1], &polygon->vertices[0]);
  vector_subtract(&b, &polygon->vertices[polygon->n - 1], &polygon->vertices[0]);

  vector_cross_product(normal_out, &a, &b);
}

/*
 * Project a vector onto the canvas.
 */
 void _project_vertex(SimpleRenderer* self,
                      Vector* vertex,
                      Camera* camera,
                      int display_width,
                      int display_height)
 {
   float z;

   matrix_left_multiply_vector(camera_get_location_transform(camera), vertex);

   // XXX XXX Are we missing 'look' transform ?

   // The matrix doesn't get the Z ordinates right and I don't see how to
   // fix it. This does the trick.
   z = (2 * vertex->z - (self->_far_plane + self->_near_plane) )
       / (self->_near_plane - self->_far_plane);

   //matrix_left_multiply_vector(view_transform, &vertex);
   //matrix_left_multiply_vector(camera_location_transform, &vertex);
   matrix_left_multiply_vector(&self->_perspective_transform, vertex);

   //printf("*%f,%f,%f,%f\n", vertex.x, vertex.y, vertex.z, vertex.w);

   vector_scale_is(vertex, 1.0 / vertex->w);

   // Overwrite the matrix-calculated z with our calculation from above.
   vertex->z = z;

   //printf("%f,%f,%f,%f\n", vertex->x, vertex->y, vertex->z, vertex->w);

   // Projection places view at [-1,-1] -> [+1,+1]
   vertex->x = (vertex->x / 2. * display_width) + display_width / 2.0;
   vertex->y = (vertex->y / 2. * display_width)
               + display_width / 2.0 - display_height / 2;
 }

/*
 * Initialize the renderer with an array of polygons that make up the form.
 */
void sr_init(SimpleRenderer* self,
             const Polygon* polygons,
             size_t n,
             float near_plane,
             float far_plane,
             float near_plane_width)
{
  self->_near_plane = near_plane;
  self->_far_plane = far_plane;
  self->_polygons = polygons;
  self->_num_polygons = n;

  /*
   * Calculate normal vectors for backface culling algo.
   */
  if (self->_normals != NULL) { free(self->_normals); }

  self->_normals = malloc(sizeof(Vector) * n);

  for (size_t i = 0; i < n; ++i)
  {
    _calc_normal(&self->_normals[i], &self->_polygons[i]);
    printf("normal %zu: %s\n", i, vector_to_str(&self->_normals[i]));
  }


  /*
   * Calculate the perspective transform matrix.
   */
  matrix_tf_perspective(&self->_perspective_transform,
                        near_plane,
                        far_plane,
                        near_plane_width,
                        near_plane_width);
}

void sr_render(SimpleRenderer* self, Camera* camera, Display* display)
{
  Vector prev_vertex, first_vertex;
  int w = display_get_width(display);
  int h = display_get_height(display);

  for (int poly_index = 0; poly_index < self->_num_polygons; ++poly_index)
  {
    const Polygon* poly = &self->_polygons[poly_index];

    first_vertex = poly->vertices[0];

    _project_vertex(self, &first_vertex, camera, w, h);

    vector_copy_is(&prev_vertex, &first_vertex);

    for (int vertex_index = 1; vertex_index < poly->n; ++vertex_index)
    {
      Vector vertex = poly->vertices[vertex_index];

      _project_vertex(self, &vertex, camera, w, h);

      display_draw_line(display, prev_vertex.x, prev_vertex.y,
                                 vertex.x, vertex.y);
      vector_copy_is(&prev_vertex, &vertex);
    }

    display_draw_line(display, prev_vertex.x, prev_vertex.y,
                               first_vertex.x, first_vertex.y);
  }
}
