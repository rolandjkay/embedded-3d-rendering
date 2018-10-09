#ifndef _Vector_H
#define _Vector_H

typedef struct
{
  float x, y, z, w;
} Vector ;

#define VECTOR_INITW(x,y,z,w) {x,y,z,w}
#define VECTOR_INIT(x,y,z) {x,y,z, 1.0}

void vector_init(Vector* self, float x, float y, float z, float w);

void vector_scale(Vector* vector, float factor);
void vector_normalize(Vector* vector);
void vector_copy(Vector* dst, const Vector* src);
void vector_cross_product(Vector* dst, const Vector* u, const Vector* v);
float vector_dot_product(const Vector* u, const Vector* v);
void vector_subtract(Vector* dst, const Vector* u, const Vector* v);

void vector_to_log(Vector* self);

#endif
