/*
 * Display a test card of the SSD1306 display
 */
#ifndef F_CPU
#define F_CPU 16000000UL // or whatever may be your frequency
#endif

#include <avr/io.h>
#include <util/delay.h>                // for _delay_ms()
#include <stdlib.h>
#include <stdbool.h>

#include "../avr-defs.h"
#include "usart.h"

int main(void)
{
  SET_BIT(DDRC, DDC5);             // Arduino pin A5 as output (PC5)

  PIN_HIGH(PORTC, PORTC5);   // LED on

  usart_init();

  while (true)
  {
    usart_transmit('A');
    usart_write_string("This is a test tranmission...\n");
    _delay_ms(100);
    PIN_LOW(PORTC, PORTC5);  // LED off
    usart_transmit('B');
    usart_write_string("...from Mars\n");
    _delay_ms(100);
    PIN_HIGH(PORTC, PORTC5);  // LED off
  }
}
