#include <stdio.h>
#include <math.h>
#include "fixed_point.h"
#include "fix8_matrix.h"
#include "fix8_vector.h"
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

static void mult_overflow_test()
{
  fix8_t result = fix8_mul(-128, 127);

  printf("-1 x -1 = %f\n", result / 128.0);

  for (int i = -128; i <128; ++i) {
    for (int j = -128; j < 128; ++j) {
      int16_t r = (int16_t)i * (int16_t)j ;
      if (((r>>1) & 0x4000) ^ (r & 0x4000))
        printf("%f x %f = %f: %x\n", i/128.f, j/128.f, r / 128.0f / 128.f, r);
    }
  }
}

/*static void fix16_sqrt_test()
{
  printf("In\tOut\t\tIn\t\tOut\t\tCorrect\n");
  printf("%d\t%d\t\t%f\t%f\t%f\n", 64,   int16_sqrt(64), 64., int16_sqrt(64)/1.,8.); //112
  printf("%d\t%d\t\t%f\t%f\t%f\n", 8192, int16_sqrt(8192), 8192/16384., int16_sqrt(8192)/128., sqrt(8192/16384.)); //112
  printf("%d\t%d\t\t%f\t%f\t%f\n", 72,   int16_sqrt(11422), 11422/16384., int16_sqrt(11422)/128., sqrt(11422/16384.)); // 96
}*/


/*
 * Test the fixed-point maths routines
 */
 int main()
 {
   printf("%d %hhd\n", (int)FLT_TO_FIX(0.25), fix8_cos(FLT_TO_FIX(0.25)));
   mult_overflow_test();
   //rotation_test();
   //cosine_test();
   //sine_test();
   //fix8_x_int16_mul_test();
   return 0;
 }
