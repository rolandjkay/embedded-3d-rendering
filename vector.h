#ifndef _Vector_H
#define _Vector_H

typedef struct
{
  float x, y, z;
} Vector ;

#define VECTOR_INIT(x,y,z,w) {x,y,z}

void vector_init(Vector* self, float x, float y, float z);

void vector_scale(Vector* vector, float factor);
void vector_normalize(Vector* vector);
void vector_copy(Vector* dst, const Vector* src);
void vector_cross_product(Vector* dst, const Vector* u, const Vector* v);
float vector_dot_product(const Vector* u, const Vector* v);
void vector_subtract(Vector* dst, const Vector* u, const Vector* v);

void vector_to_log(Vector* self);

#endif
