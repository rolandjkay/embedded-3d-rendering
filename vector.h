#ifndef _Vector_H
#define _Vector_H

#include <stdint.h>
#include "fixed_point.h"

/*
 * 3D float vector
 */
typedef struct
{
  float x, y, z;
} Vector ;

#define VECTOR_INIT(x,y,z,w) {x,y,z}

void vector_init(Vector* self, float x, float y, float z);

void vector_scale(Vector* vector, float factor);
void vector_normalize(Vector* vector);
//void vector_copy(Vector* dst, const Vector* src);
void vector_cross_product(Vector* dst, const Vector* u, const Vector* v);
float vector_dot_product(const Vector* u, const Vector* v);
//void vector_subtract(Vector* dst, const Vector* u, const Vector* v);

// While we are juggling vector with different underlying types, it is useful
// to have a 'subtract' macro which is indendent of the underlying type; like
// a templated C++ function.
#define vector_subtract(dst, u, v) { (dst)->x = (u)->x - (v)->x;  (dst)->y = (u)->y - (v)->y; (dst)->z = (u)->z - (v)->z;}
#define vector_copy(dst, src) { (dst)->x = (src)->x; (dst)->y = (src)->y; (dst)->z = (src)->z; }

void vector_to_log(Vector* self);


/*
 * 2D 8 bit integer vector
 */
 typedef struct
 {
   int8_t x, y;
 } int8_vector_t;

/*
* 3D 16 bit integer vector
*/
typedef struct
{
  int16_t x, y, z;
} int16_vector_t;

/*
 * 3D 8 bit fixed-point vector
 */
typedef struct
{
  fix8_t x, y, z;
} fix8_vector_t;

/*
 * u and v must be normalized vectors with 8 bit fixed point elements:
 *
 * This way, the output cannot overflow and will be equal to the cosine of
 * the angle between the vectors.
 */
static inline fix8_t fix8_vector_normal_dot_product(const fix8_vector_t* u,
                                                    const fix8_vector_t* v)
{
  return fix8_mul(u->x, v->x) + fix8_mul(u->y, v->y) + fix8_mul(u->z, v->z);
}

static inline void fix8_vector_copy(fix8_vector_t* dst, const fix8_vector_t* src)
{
  dst->x = src->x; dst->y = src->y; dst->z = src->z;
}


#endif
