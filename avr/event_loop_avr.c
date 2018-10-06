#include "event_loop_avr.h"

#ifndef F_CPU
#define F_CPU 16000000UL // or whatever may be your frequency
#endif

#include <stdbool.h>
#include <util/delay.h>                // for _delay_ms()

void event_loop_run(EventLoopUpdateFunc* update_func)
{
  uint32_t clock = 0; // We pass this timestamp to the update function.

  // This is the world's mose useless event loop.
  while (true)
  {
    update_func(clock);
    _delay_ms(100);                // wait 100 milliseconds
    clock+=100;
  }
}
