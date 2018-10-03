CFLAGS=-std=c99 -gdwarf-2

all: demo

run: demo
	./demo

demo:	main.o display.o matrix.o vector.o camera.o simple_renderer.o \
	3d_model.o sdl/display_impl_sdl.o utils.o errors.o sdl/event_loop_sdl.o \
     	font.o animation/typed_string.o
	gcc $(CFLGAS) -o demo matrix.o main.o display.o vector.o camera.o \
	                      simple_renderer.o 3d_model.o font.o \
	                      animation/typed_string.o utils.o errors.o \
	                      sdl/display_impl_sdl.o \
	                      sdl/event_loop_sdl.o \
	                      -lSDL2

clean:
	rm main.o display.o matrix.o vector.o camera.o simple_renderer.o \
	   3d_model.o font.o animation/*.o demo sdl\*.o

main.o:		main.c
	gcc $(CFLAGS) -o main.o -c main.c

errors.o:	errors.c errors.h
	gcc $(CFLAGS) -o errors.o -c errors.c

utils.o:	utils.c utils.h
	gcc $(CFLAGS) -o utils.o -c utils.c

matrix.o:	matrix.c matrix.h
	gcc $(CFLAGS) -o matrix.o -c matrix.c

camera.o:	camera.c camera.h
	gcc $(CFLAGS) -o camera.o -c camera.c

vector.o:	vector.c vector.h
	gcc $(CFLAGS) -o vector.o -c vector.c

display.o:	display.c display.h
	gcc $(CFLAGS) -c display.c

simple_renderer.o:	simple_renderer.c simple_renderer.h
	gcc $(CFLAGS) -o simple_renderer.o -c simple_renderer.c

3d_model.o:	3d_model.c 3d_model.h
	gcc $(CFLAGS) -o 3d_model.o -c 3d_model.c

font.o:	font.c font.h
	gcc $(CFLAGS) -o font.o -c font.c

animation/typed_string.o:	animation/typed_string.c animation/typed_string.h
	gcc $(CFLAGS) -o animation/typed_string.o -c animation/typed_string.c

sdl/display_impl_sdl.o:	sdl/display_impl_sdl.c sdl/display_impl_sdl.h
	gcc $(CFLAGS) -o sdl/display_impl_sdl.o -c sdl/display_impl_sdl.c

sdl/event_loop_sdl.o:	sdl/event_loop_sdl.c sdl/event_loop_sdl.h
	gcc $(CFLAGS) -o sdl/event_loop_sdl.o -c sdl/event_loop_sdl.c
