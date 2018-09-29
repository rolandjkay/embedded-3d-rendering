#include "defs.h"
#include "display.h"
#include "vector.h"
#include "matrix.h"
#include "polygon.h"
#include "camera.h"
#include "simple_renderer.h"
#include "3d_model.h"
#include <stdio.h>
#include <math.h>

// http://www.kmjn.org/notes/3d_rendering_intro.html

static Camera camera = CAMERA_INIT;

static const Vector look_point = VECTOR_INIT(0.0, 0.0, 0.0);
static float near_plane = -20.0;
static float near_plane_width = 60.0;
static float far_plane = -160.0;
static SimpleRenderer renderer;

void update(Display* display, uint32_t clock)
{
  // Move the camera
  camera_set_location(&camera, 50 * sin(clock / 1000.), 50 * cos(clock / 1000.), 130.0);
  camera_set_up_vector(&camera, 0.0, 1.0, 0.0);

  //camera_set_location(&camera, 0., 200.0, 0.0);
  //camera_set_up_vector(&camera, 0.0, 0.0, -1.0);

  camera_set_look_point(&camera, 0.0, 0.0, 0.0);
  camera_calc_transforms(&camera);

  display_cls(display);
  sr_render_scene(&renderer, &camera, display);
}

int main( int argc, char* args[] )
{
  Display* display = display_new(128, 64);
  if (!display)
  {
      fprintf(stderr, "Failed to create display: %s", get_display_error());
      return 0;
  }

  Scene scene = SCENE_INIT;
  scene.scene_objects[0].object = &viper;
  vector_init(&scene.scene_objects[0].location, 0.0, 40.0, 0.0, 1.0);
  matrix_identity(&scene.scene_objects[0].rotation_matrix);

  scene.scene_objects[1].object = &cobra;
  vector_init(&scene.scene_objects[1].location, 0.0, -20.0, 0.0, 1.0);
  matrix_tf_rotation(&scene.scene_objects[1].rotation_matrix,
                     0.0, PI, 0.0);

  printf("%s\n", matrix_to_str(&scene.scene_objects[1].rotation_matrix));

  sr_init(&renderer, &scene, near_plane, far_plane, near_plane_width);

  run_event_loop(display, update);

  free_display(display);

  return 0;
}
