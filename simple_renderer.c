#include <stdio.h>
#include <stdbool.h>
#include "simple_renderer.h"
#include "settings.h"

// http://www.kmjn.org/notes/3d_rendering_intro.html

/*
 * Project a vector onto the canvas.
 * - NB: Corrupts 'vertex'.
 */
static inline void _project_vertex(int8_vector_t* screen_vertex,
                                   Vector* vertex,
                                   const Camera* camera,
                                   int display_width,
                                   int display_height)
{
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
   screen_vertex->x = (vertex->x / 2. * display_width) + display_width / 2.0;
   screen_vertex->y = -(vertex->y / 2. * display_width) + display_height / 2.0;
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
#ifdef __AVR
  static int8_vector_t screen_vertices[40]; // XXX Maximum of 40 'points'
#else
  static int8_vector_t screen_vertices[40]; // XXX Maximum of 40 'points'
  screen_vertices[countof(screen_vertices)-1].x = 0x7f;
  screen_vertices[countof(screen_vertices)-1].y = 0x7f;
#endif
  uint16_t visible = 0;              // XXX Maximum of 16 faces !!!
  Vector unit_look_vector;

  vector_copy(&unit_look_vector, camera_get_look_vector(camera));
  vector_normalize(&unit_look_vector);

  /*
   * Check the visibility of all faces.
   */
   //usart_write_string_P(PSTR("Checking visibility of "));
   //usart_write_string_P(GET_OBJ_NAME(object));
   //usart_transmit('\n');
  for (int face_index = 0;
       face_index < GET_OBJ_NUM_FACES(scene_object->object)
         && face_index < sizeof(visible)<<3; // Bounds check
       face_index++)
  {
    Vector normal;

    //vector_copy(&normal, &object->normals[face_index]);
    Normal _normal;
    GET_OBJ_NORMAL(scene_object->object, face_index, _normal)
    normal.x = _normal.x;
    normal.y = _normal.y;
    normal.z = _normal.z;
    /*normal.x = object->normals[face_index].x;
    normal.y = object->normals[face_index].y;
    normal.z = object->normals[face_index].z;*/

    // Apply rotation
    matrix_left_multiply_vector(&scene_object->rotation_matrix, &normal);

    // Is this so that we an force a face to be visible?
    if ((normal.x == 0) && (normal.y == 0) && (normal.z == 0))
    {
      SET_BIT(visible, face_index);
    }
    else
    {
      float cos_angle;

      // The Elite face normals are not unit vectors.
      // -- Probably because it's hard to express unit vectors with 8 bit
      //    integers
      vector_normalize(&normal);

      cos_angle = vector_dot_product(&normal, &unit_look_vector);
      if (cos_angle < -0.08)
      {
        SET_BIT(visible, face_index);
      }
    }
  }

  /*
   * Project 3D world vectors to 2D screen vectors.
   */
  //usart_write_string_P(PSTR("Projecting screen_vertices\n"));
  uint8_t num_points = GET_OBJ_NUM_POINTS(scene_object->object);
  for (int vertex_index = 0; vertex_index < num_points; ++vertex_index)
  {
    Point point;
    Vector world_vertex;

    GET_OBJ_POINT(scene_object->object, vertex_index, point)

    world_vertex.x = point.x; //object->points[vertex_index].x;
    world_vertex.y = point.y; //object->points[vertex_index].y;
    world_vertex.z = point.z; //object->points[vertex_index].z;

    //snprintf(buf, 30, "~%d,%d,%d\n", (int)point.x,  (int)point.y,  (int)point.z);
    //usart_write_string(buf);

    // Apply rotation
    matrix_left_multiply_vector(&scene_object->rotation_matrix, &world_vertex);

    // Apply translation
    world_vertex.x += scene_object->location.x;
    world_vertex.y += scene_object->location.y;
    world_vertex.z += scene_object->location.z;

    _project_vertex(&screen_vertices[vertex_index],
                    &world_vertex,
                    camera,
                    display_get_width(display),
                    display_get_height(display));
  }

  //usart_write_string_P(PSTR("Drawing lines "));
  size_t num_lines = GET_OBJ_NUM_LINES(scene_object->object);
  //char buf[5];
  //snprintf(buf, 4, "%d", num_lines);
  //usart_write_string(buf);
  //usart_transmit('\n');
  for (int line_index = 0; line_index < num_lines; ++line_index)
  {
    Line line;
    GET_OBJ_LINE(scene_object->object, line_index, line)

    int8_vector_t* start_vertex = &screen_vertices[line.start_point];
    int8_vector_t* end_vertex = &screen_vertices[line.end_point];
    //Vector* start_vertex = &screen_vertices[object->lines[line_index].start_point];
    //Vector* end_vertex = &screen_vertices[object->lines[line_index].end_point];

    //snprintf(buf, 30, ">>>%f,%f,%f\n", start_vertex->x, start_vertex->y, start_vertex->z);
    //usart_write_string(buf);

    // If either of the faces that the lines join is visible, then draw it.
//    if (visible[object->lines[line_index].face1]
//        || visible[object->lines[line_index].face2])
    if (IS_BIT_SET(visible, line.face1) || IS_BIT_SET(visible, line.face2))
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

#ifndef __AVR
  if (screen_vertices[countof(screen_vertices)-1].x != 0x7f ||
      screen_vertices[countof(screen_vertices)-1].y != 0x7f)
  {
    printf("BANG!\n");
    __builtin_trap();
  }
#endif
}
