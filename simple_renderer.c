#include <stdio.h>
#include <stdbool.h>
#include "simple_renderer.h"
#include "settings.h"

// http://www.kmjn.org/notes/3d_rendering_intro.html


//static char buf[30];


/*
 * Calculate the normal facing out of the form.
 */
 /*
void _calc_normal(Vector* normal_out, const Polygon* polygon)
{
  Vector a, b;
  vector_subtract(&a, &polygon->vertices[1], &polygon->vertices[0]);
  vector_subtract(&b, &polygon->vertices[polygon->n - 1], &polygon->vertices[0]);

  vector_cross_product(normal_out, &a, &b);
}*/

/*
 * Project a vector onto the canvas.
 */
static inline void _project_vertex(SimpleRenderer* self,
                                   Vector* vertex,
                                   Camera* camera,
                                   int display_width,
                                   int display_height)
{
  float z;

   // Find vertex position relative to the camera.
   vector_subtract(vertex, vertex, camera_get_location(camera));

   matrix_left_multiply_vector(camera_get_look_transform(camera), vertex);

   /*
    * Perspective transform
    */
    vertex->x = 2. * NEAR_PLANE / NEAR_PLANE_WIDTH * vertex->x;
    vertex->y = 2. * NEAR_PLANE / NEAR_PLANE_WIDTH * vertex->y;
    vertex->x /= -vertex->z;
    vertex->y /= -vertex->z;

   // Projection places view at [-1,-1] -> [+1,+1]
   vertex->x = (vertex->x / 2. * display_width) + display_width / 2.0;
   vertex->y = -(vertex->y / 2. * display_width) + display_height / 2.0;
}

/*
 * Initialize the renderer with an array of polygons that make up the form.
 */
 #if 0
void sr_init(SimpleRenderer* self,
             const Scene* scene,
             //const Polygon* polygons,
             //size_t n,
             float near_plane,
             float far_plane,
             float near_plane_width)
{

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
#endif

void sr_render_scene(SimpleRenderer* self,
                     Scene* scene,
                     Camera* camera,
                     Display* display)
{
//  for (const SceneObject* scene_object = &self->_scene->scene_objects[0];
//       scene_object->object != NULL_PGM_PTR;
//       ++scene_object)
//  {
//    sr_render_object(self, scene_object, camera, display);
//  }
    sr_render_object(self, &scene->scene_objects[0], camera, display);
}


void sr_render_object(SimpleRenderer* self,
                      const SceneObject* scene_object,
                      Camera* camera,
                      Display* display)
{
  pgm_ptr_t object = scene_object->object;
  const Matrix* rotation_matrix = &scene_object->rotation_matrix;
  const Vector* location = &scene_object->location;

  int w = display_get_width(display);
  int h = display_get_height(display);
  Vector vertices[40];
  bool visible[40];
  Vector unit_look_vector;

  vector_copy(&unit_look_vector, camera_get_look_vector(camera));
  vector_normalize(&unit_look_vector);

  /*
   * Check the visibility of all faces.
   */
  for (int face_index = 0; face_index < GET_OBJ_NUM_FACES(object); face_index++)
  {
    Vector normal;

    //vector_copy(&normal, &object->normals[face_index]);
    Normal _normal;
    GET_OBJ_NORMAL(object, face_index, _normal)
    normal.x = _normal.x;
    normal.y = _normal.y;
    normal.z = _normal.z;
    /*normal.x = object->normals[face_index].x;
    normal.y = object->normals[face_index].y;
    normal.z = object->normals[face_index].z;*/

    // Apply rotation
    matrix_left_multiply_vector(rotation_matrix, &normal);

    // Is this so that we an force a face to be visible?
    if ((normal.x == 0) && (normal.y == 0) && (normal.z == 0))
    {
        visible[face_index] = true;
    }
    else
    {
      float cos_angle;

      // The Elite face normals are not unit vectors.
      // -- Probably because it's hard to express unit vectors with 8 bit
      //    integers
      vector_normalize(&normal);

      cos_angle = vector_dot_product(&normal, &unit_look_vector);
      visible[face_index] = (cos_angle < -0.08);
    }
  }

  uint8_t num_points = GET_OBJ_NUM_POINTS(object);

  for (int vertex_index = 0; vertex_index < num_points; ++vertex_index)
  {
    Point point;
    GET_OBJ_POINT(object, vertex_index, point)

    vertices[vertex_index].x = point.x; //object->points[vertex_index].x;
    vertices[vertex_index].y = point.y; //object->points[vertex_index].y;
    vertices[vertex_index].z = point.z; //object->points[vertex_index].z;
    vertices[vertex_index].w = 1.0;

    //snprintf(buf, 30, "~%d,%d,%d\n", (int)point.x,  (int)point.y,  (int)point.z);
    //usart_write_string(buf);

    // Apply rotation
    matrix_left_multiply_vector(rotation_matrix, &vertices[vertex_index]);

    // Apply translation
    vertices[vertex_index].x += location->x;
    vertices[vertex_index].y += location->y;
    vertices[vertex_index].z += location->z;

    // XXX Seems to corrupt memory in _project_vertex
    _project_vertex(self, &vertices[vertex_index], camera, w, h);
  }

  size_t num_lines = GET_OBJ_NUM_LINES(object);
  //foo = num_lines;
  //snprintf(buf, 10, ">>%d\n", foo);
  //usart_write_string(buf);

  for (int line_index = 0; line_index < num_lines; ++line_index)
  {
    Line line;
    GET_OBJ_LINE(object, line_index, line)

    Vector* start_vertex = &vertices[line.start_point];
    Vector* end_vertex = &vertices[line.end_point];
    //Vector* start_vertex = &vertices[object->lines[line_index].start_point];
    //Vector* end_vertex = &vertices[object->lines[line_index].end_point];

    //snprintf(buf, 30, ">>>%f,%f,%f\n", start_vertex->x, start_vertex->y, start_vertex->z);
    //usart_write_string(buf);

    // If either of the faces that the lines join is visible, then draw it.
//    if (visible[object->lines[line_index].face1]
//        || visible[object->lines[line_index].face2])
    if (visible[line.face1] || visible[line.face2])
    {
      display_draw_line(display, start_vertex->x, start_vertex->y,
                                 end_vertex->x, end_vertex->y);

/*     if (self->_object->lines[line_index].face1 == 9
         || self->_object->lines[line_index].face2 == 9)
     {
       display_draw_col_line(display, start_vertex->x, start_vertex->y,
                                      end_vertex->x, end_vertex->y);
     } */
   }
  }
}
