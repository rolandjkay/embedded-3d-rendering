CFLAGS=-std=c99 -gdwarf-2
AVR-CFLAGS=-std=c99 -Os
AVR-CFLAGS-ELF=-std=c99 # -Wl,-Map,avr.map
AVR-CC=avr-gcc
AVR-AS=avr-as
AVR-OBJCOPY=avr-objcopy
MCU=atmega328p

BUILD-DIR-MACOS=build/macos
BUILD-DIR-AVR=build/avr

DEMO-OBJ-FILENAMES=main.o display.o camera.o fix8_vector.o \
                   simple_renderer.o 3d_model.o errors.o  \
                   font.o typed_string.o fixed_point.o fix8_matrix.o

DEMO-OBJ-FILENAMES-MACOS=matrix.o vector.o event_loop_sdl.o display_impl_sdl.o
DEMO-OBJ-FILENAMES-AVR=event_loop_avr.o display_impl_ssd1306.o usart.o

DEMO-OBJ-FILES-MACOS=$(addprefix $(BUILD-DIR-MACOS)/, $(DEMO-OBJ-FILENAMES) $(DEMO-OBJ-FILENAMES-MACOS))
DEMO-OBJ-FILES-AVR=$(addprefix $(BUILD-DIR-AVR)/, $(DEMO-OBJ-FILENAMES) $(DEMO-OBJ-FILENAMES-AVR))


all: demo convert-bitmap demo-avr.hex fixed-point-tests

debug-make:
	@echo macos build dir: $(BUILD-DIR-MACOS)
	@echo macos obect files:
	@echo ------------------
	@echo $(DEMO-OBJ-FILES-MACOS)


#
# Tests
#
#
fixed-point-tests: fixed-point-tests.c $(addprefix $(BUILD-DIR-MACOS)/, fix8_matrix.o fix8_vector.o fixed_point.o)
	gcc $(CFLAGS) -o $@ $(addprefix $(BUILD-DIR-MACOS)/, fix8_matrix.o fix8_vector.o fixed_point.o) fixed-point-tests.c

#
# Utils
#

convert-bitmap: convert-bitmap.c
	gcc $(CFLAGS) -largp -o $@ $<

#
# Bitmaps
#

bitmaps:	bitmaps_ssd1306/bbc_micro_font.c bitmaps_ssd1306/cat.c

bitmaps_ssd1306/bbc_micro_font.c:	bitmaps/bbc_micro_font.bin convert-bitmap
	./convert-bitmap --symbol bbc_micro_font \
	                 bitmaps/bbc_micro_font.bin \
	                 bitmaps_ssd1306/bbc_micro_font

bitmaps_ssd1306/cat.c:	bitmaps/cat.bin convert-bitmap
	./convert-bitmap --symbol cat bitmaps/cat.bin bitmaps_ssd1306/cat

bitmaps_ssd1306/bbc_micro_font.o: bitmaps_ssd1306/bbc_micro_font.c
	gcc $(CFLAGS) -c -o $@ $<


#
# Demo
#

run: demo
	./demo

demo:	$(DEMO-OBJ-FILES-MACOS) bitmaps_ssd1306/bbc_micro_font.o
	gcc $(CFLAGS) -o demo $(DEMO-OBJ-FILES-MACOS) bitmaps_ssd1306/bbc_micro_font.o -lSDL2

clean:
	-rm $(DEMO-OBJ-FILES-MACOS)  $(DEMO-OBJ-FILES-AVR)
	-rm bitmaps_ssd1306/*.c
	-rm demo demo-avr.elf demo-avr.hex
	-rm convert-bitmap
	-rm *.lst *.s

$(BUILD-DIR-MACOS)/main.o:		main.c
	gcc $(CFLAGS) -o $@ -c $<

$(BUILD-DIR-MACOS)/fixed_point.o:	fixed_point.c fixed_point.h
	gcc $(CFLAGS) -o $@ -c $<

$(BUILD-DIR-MACOS)/errors.o:	errors.c errors.h
	gcc $(CFLAGS) -o $@ -c $<

$(BUILD-DIR-MACOS)/matrix.o:	matrix.c matrix.h
	gcc $(CFLAGS) -o $@ -c $<

$(BUILD-DIR-MACOS)/fix8_matrix.o:	fix8_matrix.c fix8_matrix.h
	gcc $(CFLAGS) -o $@ -c $<

$(BUILD-DIR-MACOS)/camera.o:	camera.c camera.h
	gcc $(CFLAGS) -o $@ -c $<

$(BUILD-DIR-MACOS)/vector.o:	vector.c vector.h
	gcc $(CFLAGS) -o $@ -c $<


$(BUILD-DIR-MACOS)/fix8_vector.o:	fix8_vector.c fix8_vector.h
	gcc $(CFLAGS) -o $@ -c $<

$(BUILD-DIR-MACOS)/display.o:	display.c display.h
	gcc $(CFLAGS) -o $@ -c $<

$(BUILD-DIR-MACOS)/simple_renderer.o:	simple_renderer.c simple_renderer.h
	gcc $(CFLAGS) -o $@ -c $<

$(BUILD-DIR-MACOS)/3d_model.o:	3d_model.c 3d_model.h
	gcc $(CFLAGS) -o $@ -c $<

$(BUILD-DIR-MACOS)/font.o:	font.c font.h
	gcc $(CFLAGS) -o $@ -c $<

$(BUILD-DIR-MACOS)/typed_string.o:	animation/typed_string.c animation/typed_string.h
	gcc $(CFLAGS) -o $@ -c $<

$(BUILD-DIR-MACOS)/display_impl_sdl.o:	macos/sdl/display_impl_sdl.c macos/sdl/display_impl_sdl.h
	gcc $(CFLAGS) -o $@ -c $<

$(BUILD-DIR-MACOS)/event_loop_sdl.o:	macos/sdl/event_loop_sdl.c macos/sdl/event_loop_sdl.h
	gcc $(CFLAGS) -o $@ -c $<



#
# AVR build
#
AVR_SRC_DIR=avr
SSD1306_SRC_DIR=avr/ssd1306
ATMEGA328P_SRC_DIR=avr/atmega328p

# Platform specific
#
$(BUILD-DIR-AVR)/event_loop_avr.o:	$(addprefix $(AVR_SRC_DIR)/, event_loop_avr.c event_loop_avr.h)
	$(AVR-CC) $(AVR-CFLAGS) -mmcu=$(MCU) -o $@ -c $<

$(BUILD-DIR-AVR)/display_impl_ssd1306.o:	$(addprefix $(SSD1306_SRC_DIR)/, display_impl_ssd1306.c display_impl_ssd1306.h)
	$(AVR-CC) $(AVR-CFLAGS) -mmcu=$(MCU) -o $@ -c $<

$(BUILD-DIR-AVR)/usart.o:	$(addprefix $(ATMEGA328P_SRC_DIR)/, usart.c usart.h)
	$(AVR-CC) $(AVR-CFLAGS) -mmcu=$(MCU) -o $@ -c $<


# Common
#
$(BUILD-DIR-AVR)/main.o:	main.c
	$(AVR-CC) $(AVR-CFLAGS) -mmcu=$(MCU) -o $@ -c $<

$(BUILD-DIR-AVR)/display.o:	display.c display.h
	$(AVR-CC) $(AVR-CFLAGS) -mmcu=$(MCU) -o $@ -c $<

$(BUILD-DIR-AVR)/fixed_point.o:	fixed_point.c fixed_point.h
	$(AVR-CC) $(AVR-CFLAGS) -mmcu=$(MCU) -o $@ -c $<

$(BUILD-DIR-AVR)/fix8_matrix.o:	fix8_matrix.c fix8_matrix.h
	$(AVR-CC) $(AVR-CFLAGS) -mmcu=$(MCU) -o $@ -c $<

$(BUILD-DIR-AVR)/fix8_vector.o:	fix8_vector.c fix8_vector.h
	$(AVR-CC) $(AVR-CFLAGS) -mmcu=$(MCU) -o $@ -c $<

$(BUILD-DIR-AVR)/camera.o:	camera.c camera.h
	$(AVR-CC) $(AVR-CFLAGS) -mmcu=$(MCU) -o $@ -c $<

$(BUILD-DIR-AVR)/simple_renderer.o:	simple_renderer.c simple_renderer.h
	$(AVR-CC) $(AVR-CFLAGS) -mmcu=$(MCU) -o $@ -c $<

$(BUILD-DIR-AVR)/3d_model.o:	3d_model.c 3d_model.h
	$(AVR-CC) $(AVR-CFLAGS) -mmcu=$(MCU) -o $@ -c $<

$(BUILD-DIR-AVR)/errors.o:	errors.c errors.h
	$(AVR-CC) $(AVR-CFLAGS) -mmcu=$(MCU) -o $@ -c $<

$(BUILD-DIR-AVR)/font.o:	font.c font.h
	$(AVR-CC) $(AVR-CFLAGS) -mmcu=$(MCU) -o $@ -c $<

$(BUILD-DIR-AVR)/typed_string.o:	animation/typed_string.c animation/typed_string.h
	$(AVR-CC) $(AVR-CFLAGS) -mmcu=$(MCU) -o $@ -c $<

# Bitmaps
#
$(BUILD-DIR-AVR)/bbc_micro_font.o:	bitmaps_ssd1306/bbc_micro_font.c bitmaps_ssd1306/bbc_micro_font.h
	$(AVR-CC) $(AVR-CFLAGS) -mmcu=$(MCU) -o $@ -c $<

demo-avr.elf:	$(DEMO-OBJ-FILES-AVR) $(BUILD-DIR-AVR)/bbc_micro_font.o
	echo $(DEMO-OBJ-FILES-AVR)
	$(AVR-CC) $(AVR-CFLAGS-ELF) -mmcu=$(MCU) -o demo-avr.elf $(DEMO-OBJ-FILES-AVR) $(BUILD-DIR-AVR)/bbc_micro_font.o

demo-avr.hex:	demo-avr.elf
	$(AVR-OBJCOPY)  -j .text -j .data -O ihex $< $@

upload-demo-avr:	demo-avr.hex
	avrdude -c arduino -P /dev/cu.usbmodem14121 -p ATMEGA328P -b 115200 -U flash:w:demo-avr.hex


# Mixed C + asm
#


simple_renderer.lst:	simple_renderer.c
	# create assembler code:
	$(AVR-CC) -mmcu=$(MCU) -S -fverbose-asm -g $(AVR-CFLAGS) simple_renderer.c -o simple_renderer.s
	# create asm interlaced with source lines:
	$(AVR-AS) -mmcu=$(MCU) -alhnd simple_renderer.s > simple_renderer.lst
