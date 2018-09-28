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

   // XXX The camera could pre-multiply the matrices, so that we only have
   // one multiplication to do here.
   matrix_left_multiply_vector(camera_get_location_transform(camera), vertex);
   matrix_left_multiply_vector(camera_get_look_transform(camera), vertex);

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
   vertex->y = -(vertex->y / 2. * display_width) + display_height / 2.0;
 }

/*
 * Initialize the renderer with an array of polygons that make up the form.
 */
void sr_init(SimpleRenderer* self,
             const Object* object,
             //const Polygon* polygons,
             //size_t n,
             float near_plane,
             float far_plane,
             float near_plane_width)
{
  self->_near_plane = near_plane;
  self->_far_plane = far_plane;
  //self->_polygons = polygons;
  //self->_num_polygons = n;
  self->_object = object;

  /*
   * Calculate normal vectors for backface culling algo.
   */
  /*if (self->_normals != NULL) { free(self->_normals); }

  self->_normals = malloc(sizeof(Vector) * n);

  for (size_t i = 0; i < n; ++i)
  {
    _calc_normal(&self->_normals[i], &self->_polygons[i]);
    printf("normal %zu: %s\n", i, vector_to_str(&self->_normals[i]));
  }*/


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
  Vector vertices[100];
  int visible[100];
  Vector unit_look_vector;

  vector_copy_is(&unit_look_vector, camera_get_look_vector(camera));
  vector_normalize_is(&unit_look_vector);

  /*
   * Check the visibility of all faces.
   */
  for (int face_index = 0; face_index < self->_object->num_faces; face_index++)
  {
    Vector normal;

    vector_copy_is(&normal, &self->_object->normals[face_index]);

    // Is this so that we an force a face to be visible?
    if ((normal.x == 0) && (normal.y == 0) && (normal.z == 0))
    {
        visible[face_index] = 1;
    }
    else
    {
      float cos_angle;

      // The Elite face normals are not unit vectors.
      // -- Probably because it's hard to express unit vectors with 8 bit
      //    integers
      vector_normalize_is(&normal);

      cos_angle = vector_dot_product(&normal, &unit_look_vector);
      if (face_index == 9)
      {
        printf("%f,%f,%f // %f,%f,%f // %f\n",
               normal.x, normal.y, normal.z,
               unit_look_vector.x, unit_look_vector.y, unit_look_vector.z,
               cos_angle
             );
      }
      visible[face_index] = (cos_angle < -0.2);
    }
  }

  for (int vertex_index = 0;
       vertex_index < self->_object->num_points;
       ++vertex_index)
  {
    //vector_copy_is(&vertices[vertex_index], self->_object->points[vertex_index]);
    vertices[vertex_index].x = self->_object->points[vertex_index].x;
    vertices[vertex_index].y = self->_object->points[vertex_index].y;
    vertices[vertex_index].z = self->_object->points[vertex_index].z;
    vertices[vertex_index].w = 1.0;

    //if (vertex_index == 0)
    {
      Vector foo;
      vector_copy_is(&foo, &vertices[vertex_index]);
      printf("---> %f/%f/%f/%f\n", foo.x, foo.y, foo.z, foo.w);
      matrix_left_multiply_vector(camera_get_location_transform(camera), &foo);
      printf("-/-> %f/%f/%f/%f\n", foo.x, foo.y, foo.z, foo.w);
    }

    _project_vertex(self, &vertices[vertex_index], camera, w, h);

    printf("-//-> %f/%f/%f/%f\n", vertices[vertex_index].x, vertices[vertex_index].y, vertices[vertex_index].z, vertices[vertex_index].w);
  }

  for (int line_index = 0;
       line_index < self->_object->num_lines;
       ++line_index)
  {
    Vector* start_vertex = &vertices[self->_object->lines[line_index].start_point];
    Vector* end_vertex = &vertices[self->_object->lines[line_index].end_point];

    // If either of the faces that the lines join is visible, then draw it.
    if (visible[self->_object->lines[line_index].face1]
        || visible[self->_object->lines[line_index].face2])
    {
      display_draw_line(display, start_vertex->x, start_vertex->y,
                                 end_vertex->x, end_vertex->y);

     if (self->_object->lines[line_index].face1 == 9
         || self->_object->lines[line_index].face2 == 9)
     {
       display_draw_col_line(display, start_vertex->x, start_vertex->y,
                                      end_vertex->x, end_vertex->y);
     }
    }
  }


  /*for (int poly_index = 0; poly_index < self->_num_polygons; ++poly_index)
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
  }*/
}
