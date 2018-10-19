#ifndef _FIXED_POINT_H
#define _FIXED_POINT_H

#ifdef __AVR
  #include "avr/pgmspace.h"
#else
  #include "macos/pgmspace.h"
#endif

// Avoid using these in the AVR build, to avoid linking float lib.
// - We have to special case 1.0, because it is out of range.
#define FLT_TO_FIX(x)  ((x) == 1.0 ? 127 : (fix8_t)((x)*128))
#define FIX_TO_FLT(x)  (((float)(x))/128)

/*
 * 7 bits fractional; 1 sign bit; i.e. 1.7 format
 *
 * NB: ((float)c)/128  will convert floating point for display
 */
typedef int8_t fix8_t;

/*
 * 14 bits fractional; 2 sign bit; i.e. 2.14 format
 *
 * Sometimes we need an intermediate format to hold the result
 * of multiplying two fix8_t types. 2.14 is an odd format, but
 * that is what we get if we don't shift left.
 */
//typedef int16_t fix16_t;

/*
 * Multiply two 1.7 fixed-point numbers to yield 1.7 result.
 */
static inline fix8_t fix8_mul(fix8_t a, fix8_t b)
{
  // -1 x -1 are the only possible operands that overflow. To avoid the
  // result -1 x -1 = -1, we explicitly return 127/128.
  if (a == -128 && b == -128)
  {
    return 127;
  }

  int16_t r = (int16_t)a * (int16_t)b;
  return r >> 7;
}

// https://www.avrfreaks.net/forum/16-x-16-bit-multiply-32-bit-result
// I don't know if this is better than the lib function :~
#define MultiS8X16to24(longRes, charIn1, intIn2) \
__asm volatile ( \
"clr r16 \n\t" \
"mulSU %A1, %A2 \n\t" \
"sbrc r1, 7 \n\t" \
"ser r16 \n\t" \
"movw %A0, r0 \n\t" \
"mov %C0, r16 \n\t" \
"mov %D0, r16 \n\t" \
"clr r16 \n\t" \
"muls %A1, %B2 \n\t" \
"sbrc r1, 7 \n\t" \
"ser r16 \n\t" \
"add %B0, r0 \n\t" \
"adc %C0, r1 \n\t" \
"adc %D0, r16 \n\t" \
"clr r1 \n\t" \
: \
"=&r" (longRes) \
: \
"a" (charIn1), \
"a" (intIn2) \
: \
"r16" \
)

/*
 * Multiply a 16 bit integer by a 1.7 fixed-point numbers to yield a 16 bit
 * integer result.
 */
static inline int16_t fix8_x_int16_mul(fix8_t a, int16_t b)
{
  // 16.0 x 1.7 yields 17.7 -> ie. a 24 bit intermediate result, before we
  // shift left. Therefore, we must store the result in a 32 bit int.
  int32_t r;

#ifndef __AVR
  r = (int16_t)a * b;
#else
  MultiS8X16to24(r, a, b);
#endif
  // r is 17.7; shift off the fractional bits to leave an integer
  return (int16_t)(r >> 7);
}

const fix8_t cos_lookup[65] PROGMEM;

static inline fix8_t fix8_cos(fix8_t theta)
{
  // Cosine is symmetric.
  if (theta > 0)
  {
    theta = -theta;
  }

  if (theta < -64)
  {
    return -pgm_read_byte(cos_lookup + 128 + theta);
  }
  else
  {
    return pgm_read_byte(cos_lookup - theta);
  }
}

static inline fix8_t fix8_sin(fix8_t theta)
{
  return fix8_cos(theta - 64);
}

// integer square root and fixed-point square root are the same.
//int8_t int16_sqrt(int16_t num);


/*
const fix8_t sqrt_lookup[128] PROGMEM;

static inline fix8_t fix8_sqrt(fix8_t x)
{
  // Catch sqrt of negative number.
  if (x & 0b10000000)
  {
#ifndef __AVR
    __builtin_trap();
#endif
    return 0;
  }

  return sqrt_lookup[x];
} */


#endif
