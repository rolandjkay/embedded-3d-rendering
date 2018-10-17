#include "fix8_vector.h"
#include "fixed_point.h"

#if 0
void fix8_vector_normalize(fix8_vector_t* self)
{
  int16_t tmp = self->x * self->x + self->y * self->y + self->z * self->z;
  fix8_t mag = int16_sqrt(tmp);

  // XXX How should FP division work ???
  self->x /= mag;
  self->y /= mag;
  self->z /= mag;
}
#endif

void fix8_vector_cross_product(fix8_vector_t* dst,
                               const fix8_vector_t* u,
                               const fix8_vector_t* v)
{
  // minus sign added because we're in a left-hand coordinate system.
  dst->x = -(fix8_mul(u->y, v->z) - fix8_mul(u->z, v->y));
  dst->y = -(fix8_mul(u->z, v->x) - fix8_mul(u->x, v->z));
  dst->z = -(fix8_mul(u->x, v->y) - fix8_mul(u->y, v->x));
}
