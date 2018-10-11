#include "event_loop_avr.h"

#ifndef F_CPU
#define F_CPU 16000000UL // or whatever may be your frequency
#endif

#include <stdbool.h>
#include <avr/io.h>
#include <util/delay.h>                // for _delay_ms()

#include "../defs.h"
#include "avr-defs.h"

void event_loop_run(EventLoopUpdateFunc* update_func)
{
  uint32_t clock = 0; // We pass this timestamp to the update function.

  // Set Arduino pin A5 (PC5) as output high to blink diagnostic LED.
  SET_BIT(DDRC, DDC5);

  // This is the world's mose useless event loop.
  while (true)
  {
    if ((clock / 1000) % 2)
      PIN_HIGH(PORTC, PORTC5);       // PC0 = High = Vcc
    else
      PIN_LOW(PORTC, PORTC5);       // PC0 = High = Vcc

    update_func(clock);
    _delay_ms(100);                // wait 100 milliseconds
    clock+=100;
  }
}
