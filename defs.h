#ifndef _DEFS_H
#define _DEFS_H

#include <stdint.h>

#define countof(x) (sizeof(x) / sizeof(x[0]))
#define MIN(x, y) (((x) < (y)) ? (x) : (y) )
#define PI 3.1415926535

/*
 * Because (e.g.) ships reside in program space, we don't want to use
 * a C pointer type as the type for a pointer to ship. This is because this
 * allows us to write valid C code that will fail at runtime because the data
 * is not in RAM. If we use uint16_t, we force a compile error if we try to
 * write ship_ptr->name instead of prg_read_word(&ship_ptr->name), for example.
 */
#ifdef __AVR
  typedef uint16_t pgm_ptr_t;
#else
  typedef const void* pgm_ptr_t;

  // We don't want to use these on AVR to avoid linking float lib.
#  define FLT_TO_FIX(x)  ((fix8_t)((x)*64))
#  define FIX_TO_FLT(x)  (((float)(x))/64)
#endif

#define NULL_PGM_PTR 0

#define SET_BIT(port, bit) ((port) |= (1<<bit))
#define CLEAR_BIT(port, bit) ((port) &= ~(1<<bit))

#define IS_BIT_SET(port, bit) ((port) & (1<<bit))

#endif
