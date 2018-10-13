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
   vector_subtract(vertex, vertex, &camera->camera_location);

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

static inline void _fx_project_vertex(int8_vector_t* screen_vertex,
                                      int16_vector_t* world_vertex,
                                      const Camera* camera,
                                      int display_width,
                                      int display_height)
{
   // Find vertex position relative to the camera.
   vector_subtract(world_vertex, world_vertex, &camera->camera_location);


   fix8_matrix_left_multiply_int16_vector(&camera->_fx_camera_look_transform, world_vertex);


   /*
    * Let's choose near_plane_width to be 128 so that our world coordinates
    * map 1-to-1 to pixels on the screen. Let's choose near_plane to be 128,
    * because (with )

       x = x / (NEAR_PLANE_WIDTH/2);                          10 / 64
       x = x * NEAR_PLANE / -z                                10/64 * 64/64
       x = x * HALF_DISPLAY_WIDTH + HALF_DISPLAY_WIDTH        10

       then, this becomes
       x = (x / 64 * 64 / -z) * 64 + 64)
       x = (x / -z)*64 + 64
       x = 64*x / -z + 64

       y = (y / 64 * 64 / -z) * 64 + 32)
       y = (y / -z)*64 + 32
       y = 64*x / -z + 32
     */
  screen_vertex->x = (world_vertex->x << 6) / -world_vertex->z + 64;
  screen_vertex->y = (world_vertex->y << 6) / -world_vertex->z + 32;

   /*
    * Perspective transform
    */
  /*  #define _NEAR_PLANE 6
    #define _NEAR_PLANE_WIDTH 6
    vertex->x <<= (_NEAR_PLANE - _NEAR_PLANE_WIDTH + 1)
    vertex->y <<= (_NEAR_PLANE - _NEAR_PLANE_WIDTH + 1)
    vertex->x /= -vertex->z;
    vertex->y /= -vertex->z;

   // Projection places view at [-1,-1] -> [+1,+1]
   register uint8_t half_display = display_width >> 1;
   screen_vertex->x = (vertex->x / 2. * display_width) + (display_width >> 1);
   screen_vertex->y = -(vertex->y / 2. * display_width) + (display_height >> 2);*/
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

/*static fix8_vector_t normals = {
{0,57,29},
{-19,59,17},
{19,59,17},
{-18,59,16},
{18,59,16},
{-18,61,0},
{18,61,0},
{-33,55,0},
{33,55,0},
{0,0,-64},
{-10,-62,13},
{0,-63,13},
{10,-62,13}
};*/

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

  ////////////////////////////////////////////////
  // START FIX8
  static int8_vector_t fx_screen_vertices[40]; // XXX Maximum of 40 'points'

  uint16_t fx_visible = 0;              // XXX Maximum of 16 faces !!!
  fix8_vector_t fx_unit_look_vector;
  fx_unit_look_vector.x = (fix8_t)(unit_look_vector.x * 64);
  fx_unit_look_vector.y = (fix8_t)(unit_look_vector.y * 64);
  fx_unit_look_vector.z = (fix8_t)(unit_look_vector.z * 64);

  //fix8_matrix_to_log(&scene_object->fx_rotation_matrix);
  //matrix_to_log(&scene_object->rotation_matrix);

  for (int face_index = 0;
       face_index < GET_OBJ_NUM_FACES(scene_object->object)
         && face_index < sizeof(fx_visible)<<3; // Bounds check
       face_index++)
  {
    Vector tmp;
    fix8_vector_t fx_normal;

    Normal _fx_normal;
    GET_OBJ_NORMAL(scene_object->object, face_index, _fx_normal)
    tmp.x = _fx_normal.x;
    tmp.y = _fx_normal.y;
    tmp.z = _fx_normal.z;
    vector_normalize(&tmp);
    fx_normal.x = (fix8_t)(tmp.x * 64);
    fx_normal.y = (fix8_t)(tmp.y * 64);
    fx_normal.z = (fix8_t)(tmp.z * 64);

    // Apply rotation
    fix8_matrix_left_multiply_vector(&scene_object->fx_rotation_matrix, &fx_normal);

    // Is this so that we an force a face to be visible?
    if ((fx_normal.x == 0) && (fx_normal.y == 0) && (fx_normal.z == 0))
    {
      SET_BIT(fx_visible, face_index);
    }
    else
    {
      fix8_t cos_angle;

      cos_angle = fix8_vector_normal_dot_product(&fx_normal, &fx_unit_look_vector);
      if (cos_angle < -0.08)
      {
        SET_BIT(fx_visible, face_index);
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
    // Start with a vector to the 'centre' of the object
    int16_vector_t world_vertex = scene_object->location;

    // Get unrotated point
    fix8_vector_t point;
    GET_OBJ_POINT(scene_object->object, vertex_index, point)

    // Apply rotation to point
    fix8_matrix_left_multiply_vector(&scene_object->fx_rotation_matrix, &point);

    // Add offset to the current 'point' to get the location vector of the
    // vertex in world space.
    // - Types are a bit funny here. However, adding a fix8_t to an int16_t
    //   effectively left shifts the fix8_t by the number of bits in the
    //   fractional part. So, if there were 7 fractional its, we scale from
    //   (-1,+1) to (-128,+128). Thus, the size of a ship in world space is
    //   exactly 256, which is exactly the same as it would be if we consider
    //   the 'points' to be 8 bit integers in world space.
    world_vertex.x += point.x;
    world_vertex.y += point.y;
    world_vertex.z += point.z;

    _fx_project_vertex(&fx_screen_vertices[vertex_index],
                       &world_vertex,
                       camera,
                       display_get_width(display),
                       display_get_height(display));
  }


  // END FIX8
  ////////////////////////////////////////////////


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
  //uint8_t num_points = GET_OBJ_NUM_POINTS(scene_object->object);
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

    int8_vector_t* start_vertex = &fx_screen_vertices[line.start_point];
    int8_vector_t* end_vertex = &fx_screen_vertices[line.end_point];
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
