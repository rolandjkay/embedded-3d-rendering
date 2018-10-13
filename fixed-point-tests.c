#include <stdio.h>
#include "fixed_point.h"
#include "matrix.h"
#include "vector.h"
#include "defs.h"





static void rotation_test()
{
  fix8_vector_t v = {0, FLT_TO_FIX(1.0), 0};
  fix8_vector_t u;
  fix8_matrix_t m;

  fix8_vector_copy(&u, &v);

  // Rotate about y axis
  fix8_matrix_tf_rotation(&m, 0, 0, FLT_TO_FIX(0.25));
  fix8_matrix_left_multiply_vector(&m, &u);

  fix8_matrix_to_log(&m);

  printf("[%f,%f,%f] rotated 45deg about z gives [%f,%f,%f]\n",
         FIX_TO_FLT(v.x), FIX_TO_FLT(v.y), FIX_TO_FLT(v.z),
         FIX_TO_FLT(u.x), FIX_TO_FLT(u.y), FIX_TO_FLT(u.z));

}

/*
 * Test the fixed-point maths routines
 */
 int main()
 {
   printf("%d %hhd\n", (int)FLT_TO_FIX(0.25), fix8_cos(FLT_TO_FIX(0.25)));
   rotation_test();
   return 0;
 }
