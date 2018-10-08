CFLAGS=-std=c99
CC=avr-gcc
OBJCOPY=avr-objcopy
MCU=atmega328p

all: usart-test.hex

clean:
	-rm usart-test.elf usart-test.o usart-test.hex

upload-usart-test:	usart-test.hex
	avrdude -c arduino -P /dev/cu.usbmodem14121 -p ATMEGA328P -b 115200 -U flash:w:usart-test.hex

usart-test.o:	usart-test.c
	$(CC) $(CFLAGS) -g -Os -mmcu=$(MCU) -c usart-test.c

usart-test.elf:	usart-test.o usart.c
	$(CC) -g -mmcu=$(MCU) -o usart-test.elf usart-test.o usart.c

usart-test.hex:	usart-test.elf
	$(OBJCOPY) -j .text -j .data -O ihex usart-test.elf usart-test.hex
