#include <stdint.h>

#ifndef F_CPU
#define F_CPU 16000000UL // or whatever may be your frequency
#endif

#include <avr/io.h>
#include <avr/pgmspace.h>
#include "../avr-defs.h"
#include "../../defs.h"

/* Settings */
#define _BAUD 9600					// Baud rate (9600 is default)
#define _DATA 0x03					// Number of data bits in frame = byte tranmission
#define _UBRR (F_CPU/16)/_BAUD - 1		// Used for UBRRL and UBRRH

/* Useful macros */
#define TX_START()   SET_BIT(UCSR0B, TXEN0)	// Enable TX
#define TX_STOP()    CLEAR_BIT(UCSR0B, ~TXEN0)	// Disable TX
#define RX_START()   SET_BIT(UCSR0B, RXEN0)	// Enable RX
#define RX_STOP()    CLEAR_BIT(UCSR0B, RXEN0)	// Disable RX
#define COMM_START() TX_START(); RX_START()	// Enable communications

void usart_init()
{
  SET_BIT(PORTD, PORTD1);
  CLEAR_BIT(PORTD, PORTD0);

  // Set baud rate; lower byte and top nibble
	UBRR0H = ((_UBRR) & 0xF00);
	UBRR0L = (uint8_t) ((_UBRR) & 0xFF);

  TX_START();
	RX_START();

  // Set frame format = 8-N-1
	UCSR0C = (_DATA << UCSZ00);
}


void usart_transmit(uint8_t data ) {
  /* Wait for empty transmit buffer */
  while ( !( UCSR0A & (1<<UDRE0)) );
  /* Put data into buffer, sends the data */
  UDR0 = data;
}

// function to receive data
uint8_t uart_recieve (void)
{
    while(!(UCSR0A) & (1<<RXC0));                   // wait while data is being received
    return UDR0;                                   // return 8-bit data
}

void usart_write_string(const char* str)
{
  for (; *str; ++str)
  {
    usart_transmit((uint8_t)*str);
  }
}

void usart_write_string_P(pgm_ptr_t str)
{
  for (; pgm_read_byte(str); ++str)
  {
    usart_transmit(pgm_read_byte(str));
  }
}
