#ifndef _FIXED_POINT_H
#define _FIXED_POINT_H

#ifdef __AVR
  #include "avr/pgmspace.h"
#else
  #include "macos/pgmspace.h"
#endif

/*
 * 1 bit integer part; 6 bits fractional; 1 sign bit
 *
 * NB: ((float)c)/64  will convert floating point for display
 */
typedef int8_t fix8_t;

/*
 * Multiply to 2.6 fixed-point numbers to yield 2.6 result.
 */
static inline fix8_t fix8_mul(fix8_t a, fix8_t b)
{
  int16_t r = (int16_t)a * (int16_t)b;
  return r >> 6;
}
/*
 * Multiply a 16 bit integer by a 2.6 fixed-point numbers to yield a 16 bit
 * integer result.
 */
static inline int16_t int16_x_fix8_mul(fix8_t a, int16_t b)
{
  int16_t r = (int16_t)a * b;
  // r is 18.6; shift off the fractional bits to leave an integer
  return r >> 6;
}

const fix8_t cos_lookup[34] PROGMEM;

static inline fix8_t fix8_cos(fix8_t theta)
{
  // Cosine is symmetric.
  if (theta > 0)
  {
    theta = -theta;
  }

  if (theta < -96)
  {
    return pgm_read_byte(cos_lookup + 128 - -theta);
  }
  if (theta < -64)
  {
    return -pgm_read_byte(cos_lookup + -theta - 64);
  }
  else if (theta < -32)
  {
    return -pgm_read_byte(cos_lookup + 64 - -theta);
  }
  else
  {
    return pgm_read_byte(cos_lookup + -theta);
  }
}

static inline fix8_t fix8_sin(fix8_t theta)
{
  return fix8_cos(theta - 32);
}

#endif
