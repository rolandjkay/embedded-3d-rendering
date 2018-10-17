#include "fixed_point.h"

/*
 * Tables contains:    which is the same as

 * cos(0/128*PI)       cos(0/128*PI)
 * cos(-1/128*PI)      cos(1/128*PI)
 * cos(-2/128*PI)      cos(2/128*PI)
 * cos(-3/128*PI)
 * cos(-4/128*PI)
 * cos(-5/128*PI)
 * cos(-6/128*PI)
 * cos(-7/128*PI)
 * cos(-8/128*PI)
 * ...                 ...
 * cos(-63/128*PI)     cos(63/128*PI)
 * cos(-64/128*PI)     cos(64/128*PI)
 */
const fix8_t cos_lookup[65] PROGMEM = {
127,127,127,127,127,127,126,126,
125,124,124,123,122,121,120,119,
118,117,115,114,112,111,109,108,
106,104,102,100, 98, 96, 94, 92,
 90, 88, 85, 83, 81, 78, 76, 73,
 71, 68, 65, 63, 60, 57, 54, 51,
 48, 46, 43, 40, 37, 34, 31, 28,
 24, 21, 18, 15, 12,  9,  6,  3,
  0
};

/*
 * Compute the square root of x
 * See: https://briangordon.github.io/2014/06/sqrts-and-fixed-points.html
 */
/*fix8_t fix8_sqrt(fix8_t a)
{
  fix8_t xn_plus_1;
  fix8_t xn = 0b01000000;  // i.e. 0.5

  while(1)
  {
    xn_plus_1 = (xn + a / xn) >> 1;
    if (xn_plus_1 == xn)
      return xn * 11;
    xn = xn_plus_1;
  }
} */

/*const fix8_t sqrt_lookup[128] PROGMEM = {
  0,  11,  16,  19,  22,  25,  27,  29,  32,  33,  35,  37,  39,  40,  42,  43,
 45,  46,  48,  49,  50,  51,  53,  54,  55,  56,  57,  58,  59,  60,  61,  62,
 64,  64,  65,  66,  67,  68,  69,  70,  71,  72,  73,  74,  75,  75,  76,  77,
 78,  79,  80,  80,  81,  82,  83,  83,  84,  85,  86,  86,  87,  88,  89,  89,
 90,  91,  91,  92,  93,  93,  94,  95,  96,  96,  97,  97,  98,  99,  99, 100,
101, 101, 102, 103, 103, 104, 104, 105, 106, 106, 107, 107, 108, 109, 109, 110,
110, 111, 112, 112, 113, 113, 114, 114, 115, 115, 116, 117, 117, 118, 118, 119,
119, 120, 120, 121, 121, 122, 122, 123, 123, 124, 124, 125, 125, 126, 126, 127
};*/


#if 0
/*
 * http://en.wikipedia.org/wiki/Methods_of_computing_square_roots#Binary_numeral_system_.28base_2.29
 */
int8_t int16_sqrt(int16_t num)
{
  int16_t res = 0;
  int16_t bit = 1 << 14; // The second-to-top bit is set: 1 << 30 for 32 bits

  // "bit" starts at the highest power of four <= the argument.
  while (bit > num)
      bit >>= 2;

  while (bit != 0) {
      if (num >= res + bit) {
          num -= res + bit;
          res += bit << 1;
      }

      res >>= 1;
      bit >>= 2;
  }
  return res;
}
#endif


/*
 * Adapted from: https://github.com/PetteriAimonen/libfixmath/blob/master/libfixmath/fix16_sqrt.c
 * The square root algorithm is quite directly from
 * http://en.wikipedia.org/wiki/Methods_of_computing_square_roots#Binary_numeral_system_.28base_2.29
 * An important difference is that it is split to two parts
 * in order to use only 32-bit operations.
 *
 * Note that for negative numbers we return -sqrt(-inValue).
 * Not sure if someone relies on this behaviour, but not going
 * to break it for now. It doesn't slow the code much overall.
 */
/*
{
	uint8_t  neg = (inValue < 0);
	uint32_t num = (neg ? -inValue : inValue);
	uint32_t result = 0;
	uint32_t bit;
	uint8_t  n;

	// Many numbers will be less than 15, so
	// this gives a good balance between time spent
	// in if vs. time spent in the while loop
	// when searching for the starting value.
	if (num & 0xFFF00000)
		bit = (uint32_t)1 << 30;
	else
		bit = (uint32_t)1 << 18;

	while (bit > num) bit >>= 2;

	// The main part is executed twice, in order to avoid
	// using 64 bit values in computations.
	for (n = 0; n < 2; n++)
	{
		// First we get the top 24 bits of the answer.
		while (bit)
		{
			if (num >= result + bit)
			{
				num -= result + bit;
				result = (result >> 1) + bit;
			}
			else
			{
				result = (result >> 1);
			}
			bit >>= 2;
		}

		if (n == 0)
		{
			// Then process it again to get the lowest 8 bits.
			if (num > 65535)
			{
				// The remainder 'num' is too large to be shifted left
				// by 16, so we have to add 1 to result manually and
				// adjust 'num' accordingly.
				// num = a - (result + 0.5)^2
				//	 = num + result^2 - (result + 0.5)^2
				//	 = num - result - 0.5
				num -= result;
				num = (num << 16) - 0x8000;
				result = (result << 16) + 0x8000;
			}
			else
			{
				num <<= 16;
				result <<= 16;
			}

			bit = 1 << 14;
		}
	}


#ifndef FIXMATH_NO_ROUNDING
	// Finally, if next bit would have been 1, round the result upwards.
	if (num > result)
	{
		result++;
	}
#endif

	return (neg ? -(fix16_t)result : (fix16_t)result) >> 7;
}*/
