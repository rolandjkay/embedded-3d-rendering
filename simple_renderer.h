#ifndef _SIMPLE_RENDERER

#include "vector.h"
#include "matrix.h"
#include "camera.h"
#include "display.h"
#include "3d_model.h"
#include <stdlib.h>


typedef struct
{
} SimpleRenderer;

#define SR_INIT

/*
 * Initialize the renderer with an array of polygons that make up the form.
 */
//void sr_init(SimpleRenderer* self);

void sr_render_scene(SimpleRenderer* self, Scene* scene,
                                           Camera* camera,
                                           Display* display);
void sr_render_object(SimpleRenderer* self,
                      const SceneObject* object,
                      Camera* camera,
                      Display* display);

#ifdef INCLUDE_FLOAT_MATHS
void sr_render_object_float(SimpleRenderer* self,
                            const SceneObject* object,
                            Camera* camera,
                            Display* display);
#endif

#endif
