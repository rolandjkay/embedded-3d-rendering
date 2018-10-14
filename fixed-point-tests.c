#include <stdio.h>
#include "fixed_point.h"
#include "matrix.h"
#include "vector.h"
#include "defs.h"


static void cosine_test()
{
  fix8_t theta = -128;
  printf("theta\tcos(t)\t\ttheta\tcos(t)\n");
  do {
    printf("%hhd\t%hhd\t\t%f\t%f\n",
           theta,
           fix8_cos(theta),
           FIX_TO_FLT(theta),
           FIX_TO_FLT(fix8_cos(theta))
         );
  } while (++theta != -128);
}

static void sine_test()
{
  fix8_t theta = -128;
  printf("theta\tsin(t)\t\ttheta\tsin(t)\n");
  do {
    printf("%hhd\t%hhd\t\t%f\t%f\n",
           theta,
           fix8_sin(theta),
           FIX_TO_FLT(theta),
           FIX_TO_FLT(fix8_sin(theta))
         );
  } while (++theta != -128);
}

static void rotation_test()
{
  fix8_vector_t v = {0, FLT_TO_FIX(1.0), 0};
  fix8_vector_t u;
  fix8_matrix_t m;

  fix8_vector_copy(&u, &v);

  // Rotate about z axis
  fix8_matrix_tf_rotation(&m, 0, 0, FLT_TO_FIX(0.25));
  fix8_matrix_left_multiply_vector(&m, &u);

  fix8_matrix_to_log(&m);

  printf("[%f,%f,%f] rotated 45deg about z gives [%f,%f,%f]\n",
         FIX_TO_FLT(v.x), FIX_TO_FLT(v.y), FIX_TO_FLT(v.z),
         FIX_TO_FLT(u.x), FIX_TO_FLT(u.y), FIX_TO_FLT(u.z));
}

static void fix8_x_int16_mul_test()
{
  int16_t operand16 = -256;
  fix8_t operand8 = 0x80;   // i.e. -1
  int16_t result = fix8_x_int16_mul(operand8, operand16);

  printf("-256 x -1 = %d\n", result);
}

/*
 * Test the fixed-point maths routines
 */
 int main()
 {
   printf("%d %hhd\n", (int)FLT_TO_FIX(0.25), fix8_cos(FLT_TO_FIX(0.25)));
   //rotation_test();
   //cosine_test();
   //sine_test();
   fix8_x_int16_mul_test();
   return 0;
 }
