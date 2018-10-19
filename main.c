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
                        /* look_vector = */ {0,0,-128},
                        /* up vector = */  {0,127,0}};

//static const Vector look_point = VECTOR_INIT(0.0, 0.0, 0.0);
static SimpleRenderer renderer;
static Scene scene = SCENE_INIT;
static Display display = DISPLAY_INIT;

static TypedString ship_name_animator;
static void update(uint32_t clock)
{
  //clock = 2400;
  // Calc fixed-point rotation matrix
  // - angle is in units of PI.

  fix8_t angle = ((clock % 8192) - 4096) >> 5; // = *128 / 4096

  fix8_matrix_tf_rotation(&scene.scene_objects[0].rotation_matrix,
                          angle, 0.0, angle);

#ifdef INCLUDE_FLOAT_MATHS
  matrix_tf_rotation(&scene.scene_objects[0].float_rotation_matrix,
                     FIX_TO_FLT(angle) * PI, 0.0, FIX_TO_FLT(angle) * PI);
#endif

  pgm_ptr_t ship = GET_OBJ((clock / 16384) % countof(ships));
  //pgm_ptr_t ship = GET_OBJ(SHIP_BOA);

  //usart_write_string("HELLO\n");
  //return;
  scene.scene_objects[0].object = ship;
  typed_string_init(&ship_name_animator, GET_OBJ_NAME(ship), 9/*512*/, 0, 0);

  camera_calc_transforms(&camera);

  display_cls(&display);
  sr_render_scene(&renderer, &scene, &camera, &display);
  typed_string_render(&ship_name_animator, &display, clock % 16384);

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
#ifndef __AVR
    fprintf(stderr, "Failed to initialize display %d", get_error());
#endif
    return 255;
  }

#ifdef INCLUDE_FLOAT_MATHS
  matrix_identity(&scene.scene_objects[0].float_rotation_matrix);
#endif
  fix8_matrix_identity(&scene.scene_objects[0].rotation_matrix);

  //sr_init(&renderer, &scene);

  event_loop_run(update);

  return 0;
}
