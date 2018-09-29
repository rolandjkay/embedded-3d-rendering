#ifndef _3D_MODEL_H
#define _3D_MODEL_H

#include "vector.h"
#include "matrix.h"

/************************************************************************
 ** Object
 ************************************************************************/

typedef struct
{
  int x;
  int y;
  int z;
  int dist;
  int face1;
  int face2;
  int face3;
  int face4;
} Point;

typedef struct
{
  int dist;
  int face1;
  int face2;
  int start_point;
  int end_point;
} Line;

typedef struct
{
  int num_points;
  int num_lines;
  int num_faces;
  Point* points;
  Line* lines;
  Vector* normals;
} Object;

extern Object cobra, viper;

/************************************************************************
 ** Transformed object
 ** - The object data gives a fixed orientation. We additionally need a
 **   rotation matrix to represent a real object in the scene.
 ************************************************************************/

 typedef struct
 {
   Matrix rotation_matrix;
   Object* object;
 } TObject;

 /************************************************************************
  ** Scene
  ************************************************************************/

typedef struct
{
  TObject tobjects[100];
} Scene;

#define SCENE_INIT {0};

#endif
