CFLAGS=-std=c99 -gdwarf-2
AVR-CFLAGS=-std=c99 -Os
AVR-CFLAGS-ELF=-std=c99
AVR-CC=avr-gcc
MCU=atmega328p

BUILD-DIR-MACOS=macos
BUILD-DIR-AVR=avr

DEMO-OBJ-FILENAMES=main.o display.o matrix.o vector.o camera.o \
                   simple_renderer.o 3d_model.o errors.o  \
                   font.o typed_string.o

DEMO-OBJ-FILENAMES-MACOS=event_loop_sdl.o display_impl_sdl.o
DEMO-OBJ-FILENAMES-AVR=event_loop_ssd1306.o display_impl_ssd1306.o

DEMO-OBJ-FILES-MACOS=$(addprefix $(BUILD-DIR-MACOS)/, $(DEMO-OBJ-FILENAMES) $(DEMO-OBJ-FILENAMES-MACOS))
DEMO-OBJ-FILES-AVR=$(addprefix $(BUILD-DIR-AVR)/, $(DEMO-OBJ-FILENAMES) $(DEMO-OBJ-FILENAMES-AVR))


all: demo convert-bitmap

debug-make:
	@echo macos build dir: $(BUILD-DIR-MACOS)
	@echo macos obect files:
	@echo ------------------
	@echo $(DEMO-OBJ-FILES-MACOS)

#
# Utils
#

convert-bitmap: convert-bitmap.c
	gcc $(CFLAGS) -o $@ $< -largp

#
# Bitnaps
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

$(BUILD-DIR-MACOS)/main.o:		main.c
	gcc $(CFLAGS) -o $@ -c $<

$(BUILD-DIR-MACOS)/errors.o:	errors.c errors.h
	gcc $(CFLAGS) -o $@ -c $<

$(BUILD-DIR-MACOS)/matrix.o:	matrix.c matrix.h
	gcc $(CFLAGS) -o $@ -c $<

$(BUILD-DIR-MACOS)/camera.o:	camera.c camera.h
	gcc $(CFLAGS) -o $@ -c $<

$(BUILD-DIR-MACOS)/vector.o:	vector.c vector.h
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

$(BUILD-DIR-MACOS)/display_impl_sdl.o:	sdl/display_impl_sdl.c sdl/display_impl_sdl.h
	gcc $(CFLAGS) -o $@ -c $<

$(BUILD-DIR-MACOS)/event_loop_sdl.o:	sdl/event_loop_sdl.c sdl/event_loop_sdl.h
	gcc $(CFLAGS) -o $@ -c $<



#
# AVR build
#
ssd1306/event_loop_ssd1306.o:	ssd1306/event_loop_ssd1306.c ssd1306/event_loop_ssd1306.h
	$(AVR-CC) $(AVR-CFLAGS) -mmcu=$(MCU) -o ssd1306/event_loop_ssd1306.o -c ssd1306/event_loop_ssd1306.c

demo-avr.elf:	main.o display.o matrix.o vector.o camera.o simple_renderer.o \
             	3d_model.o sdl/display_impl_sdl.o errors.o \
             	ssd1306/event_loop_ssd1306.o \
             	font.o animation/typed_string.o
	$(CC) $(AVR-CFLAGS-ELF) -mmcu=$(MCU) -o demo-avr.elf ssd1306/event_loop_ssd1306.o
