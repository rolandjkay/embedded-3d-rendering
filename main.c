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

static Camera camera = CAMERA_INIT;

//static const Vector look_point = VECTOR_INIT(0.0, 0.0, 0.0);
static SimpleRenderer renderer;
static Scene scene = SCENE_INIT;
static Display display = DISPLAY_INIT;

static TypedString ship_name_animator;
static void update(uint32_t clock)
{
  // Move the camera
  float clock_ = 0.0;
  camera_set_location(&camera, 50 * sin(clock_ / 1000.), 50 * cos(clock_ / 1000.), 257.0);
  camera_set_up_vector(&camera, 0.0, 1.0, 0.0);

  //camera_set_location(&camera, 0., 200.0, 0.0);
  //camera_set_up_vector(&camera, 0.0, 0.0, -1.0);

  matrix_tf_rotation(&scene.scene_objects[0].rotation_matrix,
                     2.*PI*clock/8000.0, 0.0, 2.*PI*clock/8000.0);

  pgm_ptr_t ship = GET_OBJ((clock / 10000) % countof(ships));

  scene.scene_objects[0].object = ship;
  typed_string_init(&ship_name_animator, GET_OBJ_NAME(ship), 350, 0, 0);

  camera_set_look_point(&camera, 0.0, 0.0, 0.0);
  camera_calc_transforms(&camera);

  display_cls(&display);
  sr_render_scene(&renderer, &scene, &camera, &display);
  typed_string_render(&ship_name_animator, &display, clock % 10000);
  //font_write_string_at_text_pos_P(&display, PSTR("ship->name"), 100, 0, 4);
  /*display_draw_line(&display, 0,0,128,64);
  display_draw_pixel(&display, 0, 16);
  display_draw_pixel(&display, 1, 17);
  display_draw_pixel(&display, 2, 18);
  display_draw_pixel(&display, 3, 19);
  display_draw_pixel(&display, 4, 20);
  display_draw_pixel(&display, 5, 21);
  display_draw_pixel(&display, 6, 22);
  display_draw_pixel(&display, 7, 23);*/
  /*font_write_string(&display, "Hello World", 11, 0, 24);
  font_write_string_P(&display, GET_SHIP_NAMEx(ship), 11, 0, 32);
  font_write_char(&display, 'H', 0, 20); */

  //font_write_string_at_text_pos(&display, ship->name, 100, 1, 0);
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

  vector_init(&scene.scene_objects[0].location, 0.0, 0.0, 0.0, 1.0);
  matrix_identity(&scene.scene_objects[0].rotation_matrix);

  //sr_init(&renderer, &scene);

  event_loop_run(update);

  return 0;
}
