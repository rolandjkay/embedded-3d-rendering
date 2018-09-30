#include "defs.h"
#include "display.h"
#include "vector.h"
#include "matrix.h"
#include "polygon.h"
#include "camera.h"
#include "simple_renderer.h"
#include "3d_model.h"
#include "font.h"
#include "animation/typed_string.h"
#include <stdio.h>
#include <math.h>

// http://www.kmjn.org/notes/3d_rendering_intro.html

static Camera camera = CAMERA_INIT;

static const Vector look_point = VECTOR_INIT(0.0, 0.0, 0.0);
static float near_plane = -50.0;
static float near_plane_width = 60.0;
static float far_plane = -160.0;
static SimpleRenderer renderer;
static Scene scene = SCENE_INIT;

static TypedString ship_name_animator;

void update(Display* display, uint32_t clock)
{
  // Move the camera
  float clock_ = 0.0;
  camera_set_location(&camera, 50 * sin(clock_ / 1000.), 50 * cos(clock_ / 1000.), 257.0);
  camera_set_up_vector(&camera, 0.0, 1.0, 0.0);

  //camera_set_location(&camera, 0., 200.0, 0.0);
  //camera_set_up_vector(&camera, 0.0, 0.0, -1.0);

  matrix_tf_rotation(&scene.scene_objects[0].rotation_matrix,
                     2.*PI*clock/8000.0, 0.0, 2.*PI*clock/8000.0);

  const Object* ship = ships[(clock / 10000) % countof(ships)];

  scene.scene_objects[0].object = ship;
  typed_string_init(&ship_name_animator, ship->name, 350, 0, 0);

  camera_set_look_point(&camera, 0.0, 0.0, 0.0);
  camera_calc_transforms(&camera);

  display_cls(display);
  sr_render_scene(&renderer, &camera, display);
  //font_write_string_at_text_pos(display, ship->name, 100, 1, 0);
  typed_string_render(&ship_name_animator, display, clock % 10000);
}

int main( int argc, char* args[] )
{
  Display* display = display_new(128, 64);
  if (!display)
  {
      fprintf(stderr, "Failed to create display: %s", get_display_error());
      return 0;
  }

  scene.scene_objects[0].object = viper;
  vector_init(&scene.scene_objects[0].location, 0.0, 0.0, 0.0, 1.0);
  matrix_identity(&scene.scene_objects[0].rotation_matrix);

  //scene.scene_objects[1].object = &cobra;
  //vector_init(&scene.scene_objects[1].location, 0.0, -20.0, 0.0, 1.0);
  //matrix_tf_rotation(&scene.scene_objects[1].rotation_matrix,
  //                   0.0, PI, 0.0);

  sr_init(&renderer, &scene, near_plane, far_plane, near_plane_width);

  run_event_loop(display, update);

  free_display(display);

  return 0;
}
