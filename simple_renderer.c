#include <stdio.h>
#include <stdbool.h>
#include "simple_renderer.h"
#include "settings.h"

// http://www.kmjn.org/notes/3d_rendering_intro.html


/*
 * Project a vector onto the canvas.
 * - NB: Corrupts 'vertex'.
 */
#ifdef INCLUDE_FLOAT_MATHS
static inline void _project_vertex(int8_vector_t* screen_vertex,
                                   Vector* vertex,
                                   const Camera* camera,
                                   int display_width,
                                   int display_height)
{
   // Find vertex position relative to the camera.
   vector_subtract(vertex, vertex, &camera->camera_location);

   matrix_left_multiply_vector(&camera->_float_camera_look_transform, vertex);

   /*
    * Perspective transform
    */
    vertex->x = 2. * NEAR_PLANE / NEAR_PLANE_WIDTH * vertex->x;
    vertex->y = 2. * NEAR_PLANE / NEAR_PLANE_WIDTH * vertex->y;
    vertex->x /= -vertex->z;  // -20/-z
    vertex->y /= -vertex->z;

   // Projection places view at [-1,-1] -> [+1,+1]
   screen_vertex->x = (vertex->x / 2. * display_width) /*+ display_width / 2.0*/;
   screen_vertex->y = -(vertex->y / 2. * display_width) /*+ display_height / 2.0*/;
}
#endif

static inline void _fx_project_vertex(int8_vector_t* screen_vertex,
                                      int16_vector_t* world_vertex,
                                      const Camera* camera,
                                      int display_width,
                                      int display_height)
{
   // Find vertex position relative to the camera.
   vector_subtract(world_vertex, world_vertex, &camera->camera_location);

   // Multiply 16 bit integers with 1.7 fixed-point and return 16 bit ints.
   fix8_matrix_left_multiply_int16_vector(&camera->_camera_look_transform, world_vertex);

   /*
    * Let's choose near_plane_width to be 128 so that our world coordinates
    * map 1-to-1 to pixels on the screen. Let's choose near_plane to be 128,
    * because (with )

       x = x / (NEAR_PLANE_WIDTH/2);                          10 / 64
       x = x * NEAR_PLANE / -z                                10/64 * -128 / -z = 20/z
       x = x * HALF_DISPLAY_WIDTH + HALF_DISPLAY_WIDTH        10/z * 128 + 64 =

       then, this becomes
       x = (x / 64 * 64 / -z) * 64 + 64)
       x = (x / -z)*64 + 64
       x = 64*x / -z + 64

       y = (y / 64 * 64 / -z) * 64 + 32)
       y = (y / -z)*64 + 32
       y = 64*x / -z + 32

       Note that we're using NEAR_PLANE of 128 (should be -128), so we
       divide by z not -z.
     */
  // This is calling __divmodhi4  :(
  screen_vertex->x = (world_vertex->x << 7) / world_vertex->z /*+ 64*/;
  screen_vertex->y = -(world_vertex->y << 7) / world_vertex->z /*+ 32*/;


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
#ifdef INCLUDE_FLOAT_MATHS
    sr_render_object_float(self, &scene->scene_objects[0], camera, display);
#endif
}

void sr_render_object(SimpleRenderer* self,
                      const SceneObject* scene_object,
                      Camera* camera,
                      Display* display)
{
#ifdef __AVR
  static int8_vector_t screen_vertices[38]; // XXX Maximum of 40 'points'
#else
  static int8_vector_t screen_vertices[40]; // XXX Maximum of 40 'points'
  screen_vertices[countof(screen_vertices)-1].x = 0x7f;
  screen_vertices[countof(screen_vertices)-1].y = 0x7f;
#endif
  uint16_t visible = 0;              // XXX Maximum of 16 faces !!!

  for (int face_index = 0;
       face_index < GET_OBJ_NUM_FACES(scene_object->object)
         && face_index < sizeof(visible)<<3; // Bounds check
       face_index++)
  {
    fix8_vector_t normal;
    GET_OBJ_NORMAL(scene_object->object, face_index, normal)

    // Apply rotation
    fix8_matrix_left_multiply_vector(&scene_object->rotation_matrix, &normal);

    // Is this so that we an force a face to be visible?
    if ((normal.x == 0) && (normal.y == 0) && (normal.z == 0))
    {
      SET_BIT(visible, face_index);
    }
    else
    {
      fix8_t cos_angle;

      cos_angle = fix8_vector_normal_dot_product(&normal, &camera->look_vector);
      if (cos_angle < -10 /*-0.08*/)
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
    // Start with a vector to the 'centre' of the object
    int16_vector_t world_vertex = scene_object->location;

    // Get unrotated point
    fix8_vector_t point;
    GET_OBJ_POINT(scene_object->object, vertex_index, point)

    // Apply rotation to point
    fix8_matrix_left_multiply_vector(&scene_object->rotation_matrix, &point);

    // Add offset to the current 'point' to get the location vector of the
    // vertex in world space.
    // - Types are a bit funny here. However, adding a fix8_t to an int16_t
    //   effectively left shifts the fix8_t by the number of bits in the
    //   fractional part. So, if there were 7 fractional bits, we scale from
    //   (-1,+1) to (-128,+128). Thus, the size of a ship in world space is
    //   exactly 256, which is exactly the same as it would be if we consider
    //   the 'points' to be 8 bit integers in world space.
    world_vertex.x += point.x;
    world_vertex.y += point.y;
    world_vertex.z += point.z;

    _fx_project_vertex(&screen_vertices[vertex_index],
                       &world_vertex,
                       camera,
                       display_get_width(display),
                       display_get_height(display));
  }


  size_t num_lines = GET_OBJ_NUM_LINES(scene_object->object);
  for (int line_index = 0; line_index < num_lines; ++line_index)
  {
    Line line;
    GET_OBJ_LINE(scene_object->object, line_index, line)

    int8_vector_t* start_vertex = &screen_vertices[line.start_point];
    int8_vector_t* end_vertex = &screen_vertices[line.end_point];

    // If either of the faces that the lines join is visible, then draw it.
    if (IS_BIT_SET(visible, line.face1) || IS_BIT_SET(visible, line.face2))
    {
      display_draw_line(display, start_vertex->x, start_vertex->y,
                                 end_vertex->x, end_vertex->y);

#ifndef __AVR
     if (line_index == 18)
     {
       display_draw_col_line(display, start_vertex->x,
                                      start_vertex->y,
                                      end_vertex->x,
                                      end_vertex->y,
                                      0b00011111
                                    );
     }
#endif

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


#ifdef INCLUDE_FLOAT_MATHS


/*
 * Used to plot normals; excluded on AVR.
 */
#ifndef __AVR
static void calc_face_mid_point(Point* dst,
                                pgm_ptr_t object,
                                int face_index)
{
  int x = 0, y = 0, z = 0, n = 0;

  // We need all of the points belonging to lines that reference the face
  size_t num_lines = GET_OBJ_NUM_LINES(object);
  for (int line_index = 0; line_index < num_lines; ++line_index)
  {
    Line line;
    GET_OBJ_LINE(object, line_index, line);

    if (line.face1 == face_index || line.face2 == face_index)
    {
      // Get unrotated start point
      fix8_vector_t point;
      GET_OBJ_POINT(object, line.start_point, point)

      x += point.x;
      y += point.y;
      z += point.z;
      ++n;

      // Get unrotated start point
      GET_OBJ_POINT(object, line.end_point, point)

      x += point.x;
      y += point.y;
      z += point.z;
      ++n;
    }
  }

  dst->x = x / n;
  dst->y = y / n;
  dst->z = z / n;
 }
#endif


/*
 * Translate an object point (as a Vector) into a world vertex, by taking into account
 * the rotation and translation of the object.
 */
static inline void
object_point_to_world_vertex_float(Vector* world_vertex,
                                   const Vector* ship_vertex,
                                   const SceneObject* scene_object)
{
  vector_copy(world_vertex, ship_vertex);

  // Apply rotation
  matrix_left_multiply_vector(&scene_object->float_rotation_matrix, world_vertex);

  // Apply translation
  world_vertex->x += scene_object->location.x;
  world_vertex->y += scene_object->location.y;
  world_vertex->z += scene_object->location.z;
}

/*
 * Floating point version of render routine.
 */
void sr_render_object_float(SimpleRenderer* self,
                            const SceneObject* scene_object,
                            Camera* camera,
                            Display* display)
{
#ifdef __AVR
  static int8_vector_t screen_vertices[38]; // XXX Maximum of 40 'points'
#else
  static int8_vector_t screen_vertices[40]; // XXX Maximum of 40 'points'
  screen_vertices[countof(screen_vertices)-1].x = 0x7f;
  screen_vertices[countof(screen_vertices)-1].y = 0x7f;
#endif
  uint16_t visible = 0;              // XXX Maximum of 16 faces !!!
  Vector unit_look_vector;

  unit_look_vector.x = FIX_TO_FLT(camera->look_vector.x);
  unit_look_vector.y = FIX_TO_FLT(camera->look_vector.y);
  unit_look_vector.z = FIX_TO_FLT(camera->look_vector.z);

  /*
   * Check the visibility of all faces.
   */
  for (int face_index = 0;
       face_index < GET_OBJ_NUM_FACES(scene_object->object)
         && face_index < sizeof(visible)<<3; // Bounds check
       face_index++)
  {
    Vector normal;

    fix8_vector_t _normal;
    GET_OBJ_NORMAL(scene_object->object, face_index, _normal)
    normal.x = FIX_TO_FLT(_normal.x);
    normal.y = FIX_TO_FLT(_normal.y);
    normal.z = FIX_TO_FLT(_normal.z);

    // Apply rotation
    matrix_left_multiply_vector(&scene_object->float_rotation_matrix, &normal);

    // Is this so that we an force a face to be visible?
    if ((normal.x == 0) && (normal.y == 0) && (normal.z == 0))
    {
      SET_BIT(visible, face_index);
    }
    else
    {
      float cos_angle;

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
  uint8_t num_points = GET_OBJ_NUM_POINTS(scene_object->object);
  for (int vertex_index = 0; vertex_index < num_points; ++vertex_index)
  {
    Point point;
    Vector world_vertex;

    GET_OBJ_POINT(scene_object->object, vertex_index, point)

    vector_copy(&world_vertex, &point);

    // Apply rotation
    matrix_left_multiply_vector(&scene_object->float_rotation_matrix, &world_vertex);

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

  size_t num_lines = GET_OBJ_NUM_LINES(scene_object->object);
  for (int line_index = 0; line_index < num_lines; ++line_index)
  {
    Line line;
    GET_OBJ_LINE(scene_object->object, line_index, line)

    int8_vector_t* start_vertex = &screen_vertices[line.start_point];
    int8_vector_t* end_vertex = &screen_vertices[line.end_point];

    // If either of the faces that the lines join is visible, then draw it.
    if (IS_BIT_SET(visible, line.face1) || IS_BIT_SET(visible, line.face2))
    {
      display_draw_col_line(display, start_vertex->x, start_vertex->y,
                                     end_vertex->x, end_vertex->y, 0xf0);

    /* if (line_index == 18)
     {
       display_draw_col_line(display, screen_vertices[line.start_point].x,
                                      screen_vertices[line.start_point].y,
                                      screen_vertices[line.end_point].x,
                                      screen_vertices[line.end_point].y,
                                      0b00011111);
     }*/

   }
  }

#ifndef __AVR
  // Render normals
  size_t num_faces = GET_OBJ_NUM_FACES(scene_object->object);
  for (int face_index = 0; face_index <num_faces; ++face_index)
  {
    Point face_mid_point;
    Vector start_world_vertex, end_world_vertex, face_mid_vertex, normal_end_vertex;

    if (face_index != 2/* && face_index != 9*/) continue;

    calc_face_mid_point(&face_mid_point, scene_object->object, face_index);
    vector_copy(&face_mid_vertex, &face_mid_point);

    object_point_to_world_vertex_float(&start_world_vertex,
                                       &face_mid_vertex,
                                       scene_object);

    fix8_vector_t normal;
    GET_OBJ_NORMAL(scene_object->object, face_index, normal);

    // NB: Cannot add normal to a ship 'Point' because we will likely overflow
    // Have to use Vector instead.
    vector_add(&normal_end_vertex, &normal, &face_mid_point);

    object_point_to_world_vertex_float(&end_world_vertex,
                                       &normal_end_vertex,
                                       scene_object);


    int8_vector_t start, end;
    _project_vertex(&start, &start_world_vertex, camera, display_get_width(display), display_get_height(display));
    _project_vertex(&end, &end_world_vertex, camera, display_get_width(display), display_get_height(display));

    display_draw_col_line(display, start.x, start.y, end.x, end.y, 0xf0);
  }
#endif

#ifndef __AVR
  if (screen_vertices[countof(screen_vertices)-1].x != 0x7f ||
      screen_vertices[countof(screen_vertices)-1].y != 0x7f)
  {
    printf("BANG!\n");
    __builtin_trap();
  }
#endif
}
#endif
