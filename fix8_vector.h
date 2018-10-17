#ifndef _FIX8_VECTOR_H
#define _FIX8_VECTOR_H

#include <stdint.h>
#include "fixed_point.h"

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

void fix8_vector_normalize(fix8_vector_t* self);

// As we are juggling vectors with different underlying types, it is useful
// to have a 'subtract' macro which is indendent of the underlying type; like
// a templated C++ function.
#define vector_subtract(dst, u, v) { (dst)->x = (u)->x - (v)->x;  (dst)->y = (u)->y - (v)->y; (dst)->z = (u)->z - (v)->z;}
#define vector_copy(dst, src) { (dst)->x = (src)->x; (dst)->y = (src)->y; (dst)->z = (src)->z; }


void fix8_vector_cross_product(fix8_vector_t* dst,
                               const fix8_vector_t* u,
                               const fix8_vector_t* v);

#endif
