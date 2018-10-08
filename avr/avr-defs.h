#ifndef _AVR_DEFS_H
#define _AVR_DEFS_H

#define PIN_HIGH(port, pin) ((port) |= (1<<pin))
#define PIN_LOW(port, pin) ((port) &= ~(1<<pin))
// Euphemisms that are more natural when setting register bits
#define SET_BIT(port, pin) ((port) |= (1<<pin))
#define CLEAR_BIT(port, pin) ((port) &= ~(1<<pin))

#endif
