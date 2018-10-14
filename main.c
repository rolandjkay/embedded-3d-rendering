#include "defs.h"
#include "display.h"
#include "vector.h"
#include "matrix.h"
#include "camera.h"
#include "simple_renderer.h"
#include "3d_model.h"
#include "font.h"
#include "errors.h"
#include "animation/typed_string.h"
#include <stdio.h>
#include <math.h>

#ifdef __AVR
# include "avr/atmega328p/usart.h"
# include "avr/event_loop_avr.h"
#else
# include "macos/sdl/event_loop_sdl.h"
#endif

static Camera camera = {/* location = */  {0,0,257/*257*/},
                        /* look_point = */ {0,0,0},
                        /* up vector = */  {0,127,0}};

//static const Vector look_point = VECTOR_INIT(0.0, 0.0, 0.0);
static SimpleRenderer renderer;
static Scene scene = SCENE_INIT;
static Display display = DISPLAY_INIT;

static TypedString ship_name_animator;
static void update(uint32_t clock)
{
  // Calc fixed-point rotation matrix
  // - angle is in units of PI.
  fix8_t angle = ((clock % 8192) - 4096) >> 5; // = *128 / 4096
  fix8_matrix_tf_rotation(&scene.scene_objects[0].fx_rotation_matrix,
                          angle, 0.0, angle);

  matrix_tf_rotation(&scene.scene_objects[0].rotation_matrix,
                     FIX_TO_FLT(angle) * PI, 0.0, FIX_TO_FLT(angle) * PI);
                    // 2.*PI*clock/8192.0, 0.0, 2.*PI*clock/8192.0);

  pgm_ptr_t ship = GET_OBJ((clock / 10000) % countof(ships));

  scene.scene_objects[0].object = ship;
  typed_string_init(&ship_name_animator, GET_OBJ_NAME(ship), 350, 0, 0);

  camera_calc_transforms(&camera);

  display_cls(&display);
  sr_render_scene(&renderer, &scene, &camera, &display);
//  typed_string_render(&ship_name_animator, &display, clock % 10000);

  display_show(&display);
}

int main( int argc, char* args[] )
{
#ifdef __AVR
  usart_init();
#endif

  scene.scene_objects[0].object = GET_OBJ(SHIP_VIPER);

  if (display_init(&display) < 0)
  {
    fprintf(stderr, "Failed to initialize display %d", get_error());
    return 255;
  }

  matrix_identity(&scene.scene_objects[0].rotation_matrix);
  fix8_matrix_identity(&scene.scene_objects[0].fx_rotation_matrix);

  //sr_init(&renderer, &scene);

  event_loop_run(update);

  return 0;
}
