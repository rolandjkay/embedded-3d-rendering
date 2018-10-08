#ifndef _Vector_H
#define _Vector_H

typedef struct
{
  float x, y, z, w;
} Vector ;

#define VECTOR_INITW(x,y,z,w) {x,y,z,w}
#define VECTOR_INIT(x,y,z) {x,y,z, 1.0}

//Vector* vector_new(float x, float y, float z, float w);
void vector_init(Vector* self, float x, float y, float z, float w);

//Vector* vector_scale(const Vector* vector, float factor);
void vector_scale_is(Vector* vector, float factor);
void vector_normalize_is(Vector* vector);
void vector_copy_is(Vector* dst, const Vector* src);
void vector_cross_product(Vector* dst, const Vector* u, const Vector* v);
float vector_dot_product(const Vector* u, const Vector* v);
void vector_subtract(Vector* dst, const Vector* u, const Vector* v);

void vector_to_log(Vector* self);

#endif
