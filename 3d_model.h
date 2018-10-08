#ifndef _3D_MODEL_H
#define _3D_MODEL_H

#include <stdint.h>
#include <stddef.h>

#ifdef __AVR
  #include "avr/pgmspace.h"
#else
  #include "macos/pgmspace.h"
#endif

#include "defs.h"
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
  pgm_ptr_t name;
  uint8_t num_points;
  uint8_t num_lines;
  uint8_t num_faces;
  pgm_ptr_t points;
  pgm_ptr_t lines;
  pgm_ptr_t normals;
} Object;

extern const Object* const ships[20] PROGMEM;
extern const Object* const misc[1] PROGMEM;

#define SHIP_COBRA 0
#define SHIP_COBRA_MK_I 1
#define SHIP_VIPER 2
#define SHIP_GECKO 3
#define SHIP_ORBIT 4
#define SHIP_TRANSPORTER 5
#define SHIP_PYTHON 6
#define SHIP_BOA 7
#define SHIP_ANACONDA 8
#define SHIP_SIDEWINDER 9
#define SHIP_MAMBA 10
#define SHIP_KRAIT 11
#define SHIP_ADDER 12
#define SHIP_ASP 13
#define SHIP_FER_DE_LANCE 14
#define SHIP_MORAY 15
#define SHIP_THARGOID 16
#define SHIP_THARGON 17
#define SHIP_CONSTRICTOR 18
#define SHIP_COUGAR 19

/*
 * Because the ships reside in program space, we don't want to use
 * (const Object*) as the type for a pointer to ship. This is because this
 * allows us to write valid C code that will fail at runtime because the data
 * is not in RAM. If we use uint16_t, we force a compile error if we try to
 * write ship_ptr->name instead of prg_read_word(&ship_ptr->name).
 */

#define GET_OBJ(index) (pgm_ptr_t)pgm_read_ptr(&ships[(index)])
#define GET_OBJ_PROP_WORD(handle, prop_name) pgm_read_word(handle + offsetof(Object, prop_name)) //   &ship->prop_name))
#define GET_OBJ_PROP_BYTE(handle, prop_name) pgm_read_byte(handle + offsetof(Object, prop_name)) //   &ship->prop_name))
#define GET_OBJ_PROP_PTR(handle, prop_name) (pgm_ptr_t)pgm_read_ptr(handle + offsetof(Object, prop_name)) //   &ship->prop_name))


/*
 * Accessing structs in flash is a nightmare!
 */
#define GET_OBJ_NAME(handle) (GET_OBJ_PROP_PTR(handle, name))
#define GET_OBJ_NUM_POINTS(handle) (GET_OBJ_PROP_BYTE(handle, num_points))
#define GET_OBJ_NUM_LINES(handle) (GET_OBJ_PROP_BYTE(handle, num_lines))
#define GET_OBJ_NUM_FACES(handle) (GET_OBJ_PROP_BYTE(handle, num_faces))

#define GET_OBJ_NORMAL(handle, index, dst) { \
  pgm_ptr_t normals = GET_OBJ_PROP_PTR(handle, normals);  \
  dst.x = pgm_read_byte(normals + index*3); \
  dst.y = pgm_read_byte(normals + index*3 + 1); \
  dst.z = pgm_read_byte(normals + index*3 + 2); \
}

#define GET_OBJ_POINT(handle, index, dst) { \
  pgm_ptr_t points = GET_OBJ_PROP_PTR(handle, points);  \
  dst.x = pgm_read_byte(points + index*3); \
  dst.y = pgm_read_byte(points + index*3 + 1); \
  dst.z = pgm_read_byte(points + index*3 + 2); \
}

#define GET_OBJ_LINE(handle, index, dst) { \
  pgm_ptr_t lines = GET_OBJ_PROP_PTR(handle, lines);  \
  dst.face1 = pgm_read_byte(lines + index*4); \
  dst.face2 = pgm_read_byte(lines + index*4 + 1); \
  dst.start_point = pgm_read_byte(lines + index*4 + 2); \
  dst.end_point = pgm_read_byte(lines + index*4 + 3); \
}

#define MISC_OBJECT_MISSILE 0


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
   pgm_ptr_t object;
 } SceneObject;

 /************************************************************************
  ** Scene
  ************************************************************************/

typedef struct
{
  SceneObject scene_objects[2];
} Scene;

#define SCENE_INIT {0};

#endif
