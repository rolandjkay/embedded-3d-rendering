#ifndef _ATMEGA328P_USART_H
#define _ATMEGA328P_USART_H

#include <stdint.h>
#include <avr/pgmspace.h>
#include "../../defs.h"

void usart_init();
void usart_transmit(uint8_t data );
uint8_t uart_recieve (void);
void usart_write_string(const char* str);
void usart_write_string_P(pgm_ptr_t str);

#endif
