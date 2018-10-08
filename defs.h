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
typedef uint16_t pgm_ptr_t;
#define NULL_PGM_PTR 0

#endif
