#ifndef _SETTINGS_H
#define _SETTINGS_H

/*
 * Projection settings
 * - These are only used by the floating point arithmetic.
 */
#define NEAR_PLANE -128.0
#define NEAR_PLANE_WIDTH  128.0

/*
 * Iffthis is defined, we will include the floating-point
 * projection alongside the fixed-point as a reference. The fixed-point
 * is only approximate, so it's useful to keep the floating-point implementation
 * for comparison.
 */
#ifndef __AVR
#  define INCLUDE_FLOAT_MATHS
#else
#  undef INCLUDE_FLOAT_MATHS
#endif

#endif
