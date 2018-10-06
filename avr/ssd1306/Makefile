CFLAGS=-std=c99
CC=avr-gcc
OBJCOPY=avr-objcopy
MCU=atmega328p

BITMAP_HEADERS=$(addprefix ../bitmaps_ssd1306/, cat.h bbc_micro_font.h)
BITMAP_SRC=$(addprefix ../bitmaps_ssd1306/, cat.c bbc_micro_font.c)

all: testcard.hex

clean:
	-rm testcard.elf testcard.o testcard.hex

upload-testcard:	testcard.hex
	avrdude -c arduino -P /dev/cu.usbmodem14121 -p ATMEGA328P -b 115200 -U flash:w:testcard.hex

testcard.o:	testcard.c $(BITMAP_HEADERS)
	$(CC) $(CFLAGS) -g -Os -mmcu=$(MCU) -c testcard.c

testcard.elf:	testcard.o $(BITMAP_SRC)
	$(CC) -g -mmcu=$(MCU) -o testcard.elf testcard.o $(BITMAP_SRC)

testcard.hex:	testcard.elf
	$(OBJCOPY) -j .text -j .data -O ihex testcard.elf testcard.hex
