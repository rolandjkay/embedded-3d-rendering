#ifndef _3D_MODEL_H
#define _3D_MODEL_H

#include "vector.h"

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

extern Object cobra;

#endif
