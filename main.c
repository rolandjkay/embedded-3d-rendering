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

// Need vertices to be arranged in anti-clockwise order from the perspective of
// someone sitting inside the form, if we are to use the backface culling algo.
static const Polygon cube[] = {
  {
    // Top plane
    {
      {-30, 30, -30, 1},
      {30, 30, -30, 1},
      {30, 30, 30, 1},
      {-30, 30, 30, 1}
    },
    4
  },
  // Bottom plane
  {
    {
      {-30, -30, -30, 1},
      {-30, -30, 30, 1},
      {30, -30, 30, 1},
      {30, -30, -30, 1}
    },
    4
  },
  {
    // Front plane
    {
      {-30, -30, -30, 1},
      {-30, 30, -30, 1},
      {30, 30, -30, 1},
      {30, -30, -30, 1}
    },
    4
  },
  {
    // Back plane
    {
      {-30, -30, 30, 1},
      {30, -30, 30, 1},
      {30, 30, 30, 1},
      {-30, 30, 30, 1}
    },
    4
  },
};

static const Polygon tunnel[] = {
  {
    {
      {30.0, 0.0,  50.0, 1.0},
      {15.0, 26.0, 50.0, 1.0},
      {-15.0, 26.0, 50.0, 1.0},
      {-30.0, 0.0, 50.0, 1.0},
      {-15.0, -25.6, 50.0, 1.0},
      {15.0 ,-25.6, 50.0, 1.0}
    },
    6
  },
  {
    {
      {30.0, 0.0,  -30.0, 1.0},
      {15.0, 26.0, -30.0, 1.0},
      {-15.0, 26.0, -30.0, 1.0},
      {-30.0, 0.0, -30.0, 1.0},
      {-15.0, -25.6, -30.0, 1.0},
      {15.0 ,-25.6, -30.0, 1.0}
    },
    6
  }
};

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

  printf("AAAA\n");
  printf("%s\n", matrix_to_str(&camera._camera_location_transform));
  printf("%s\n", matrix_to_str(&camera._camera_look_transform));
  printf("BBBB\n");

  display_cls(display);
  sr_render(&renderer, &camera, display);
}

int main( int argc, char* args[] )
{
  Display* display = display_new(128, 64);
  if (!display)
  {
      fprintf(stderr, "Failed to create display: %s", get_display_error());
      return 0;
  }

  sr_init(&renderer, &cobra, near_plane, far_plane, near_plane_width);

  run_event_loop(display, update);

  free_display(display);

  return 0;
}
