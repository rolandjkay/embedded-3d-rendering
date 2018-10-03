#ifndef _3D_MODEL_H
#define _3D_MODEL_H

#include <stdint.h>

#include "vector.h"
#include "matrix.h"

/************************************************************************
 ** Object
 ************************************************************************/

typedef struct
{
  int8_t x;
  int8_t y;
  int8_t z;
  /*int dist;
  int face1;
  int face2;
  int face3;
  int face4; */
} Point;

typedef struct
{
  //int dist;
  uint8_t face1;
  uint8_t face2;
  uint8_t start_point;
  uint8_t end_point;
} Line;

typedef struct
{
  int8_t x;
  int8_t y;
  int8_t z;
  /*int dist;
  int face1;
  int face2;
  int face3;
  int face4; */
} Normal;

typedef struct
{
  const char* name;
  uint8_t num_points;
  uint8_t num_lines;
  uint8_t num_faces;
  const Point* points;
  const Line* lines;
  const Normal* normals;
} Object;

extern const Object *cobra, *cobra1, *viper, *gecko, *orbit, *transp, *pythona;
extern const Object *boa, *anaconda, *sidewnd, *mamba, *krait, *adder, *asp2;
extern const Object *ferdlce, *moray, *thargoid, *thargon, *constrct, *cougar;
extern const Object *missile;

extern const Object* ships[20];
extern const Object* misc[1];

/************************************************************************
 ** Scene object
 ** - An object in the scene, including position and rotation.
 ** - The object data gives a fixed orientation & position. We additionally
 **   need a rotation matrix and a position to represent an object
 **   in the scene.
 ************************************************************************/

 typedef struct
 {
   Matrix rotation_matrix;
   Vector location;
   const Object* object;
 } SceneObject;

 /************************************************************************
  ** Scene
  ************************************************************************/

typedef struct
{
  SceneObject scene_objects[100];
} Scene;

#define SCENE_INIT {0};

#endif
