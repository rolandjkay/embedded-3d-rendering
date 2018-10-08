/*
 * Dummy program memory macros for when running on the host.
 *
 *
 */
#ifndef _MACOS_PGMSPACE_H
#define _MACOS_PGMSPACE_H

#include <stdint.h>
#include "../defs.h"

#define PROGMEM
#define PSTR
#define strlen_P strlen

static inline uint8_t pgm_read_byte(pgm_ptr_t ptr) { return *(uint8_t*)ptr; }
static inline uint16_t pgm_read_word(pgm_ptr_t ptr) { return *(uint16_t*)ptr; }

// Can't use pgm_read_word() for pointers, because they are different sizes
// on AVR and Mac OS.
static inline pgm_ptr_t pgm_read_ptr(pgm_ptr_t ptr) { return *(pgm_ptr_t*)ptr; }

#endif
