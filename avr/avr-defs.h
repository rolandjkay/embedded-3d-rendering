#ifndef _AVR_DEFS_H
#define _AVR_DEFS_H

// Euphemisms that are more natural when setting register bits
#define PIN_HIGH(port, pin) SET_BIT(port, pin)
#define PIN_LOW(port, pin) CLEAR_BIT(port, pin)

#endif
