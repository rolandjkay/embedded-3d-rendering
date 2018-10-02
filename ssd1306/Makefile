CFLAGS=-std=c99
CC=avr-gcc
OBJCOPY=avr-objcopy
MCU=atmega328p

all: testcard.hex

clean:
	rm generate_sprites sprites.c

generate_sprites:	generate_sprites.c sprites-input.c sprites.h
	gcc -o generate_sprites generate_sprites.c sprites-input.c

sprites.c:	generate_sprites
	./generate_sprites

upload-testcard:	testcard.hex
	avrdude -c arduino -P /dev/cu.usbmodem14121 -p ATMEGA328P -b 115200 -U flash:w:testcard.hex

testcard.o:	testcard.c
	$(CC) $(CFLAGS) -g -Os -mmcu=$(MCU) -c testcard.c

sprites.o:	sprites.c
	$(CC) $(CFLAGS) -g -Os -mmcu=$(MCU) -c sprites.c

testcard.elf:	testcard.o sprites.o
	$(CC) -g -mmcu=$(MCU) -o testcard.elf testcard.o sprites.o

testcard.hex:	testcard.elf
	$(OBJCOPY) -j .text -j .data -O ihex testcard.elf testcard.hex
