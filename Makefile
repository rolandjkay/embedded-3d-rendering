CFLAGS=-std=c99 -gdwarf-2

all: demo

run: demo
	./demo

demo:	main.o display.o matrix.o vector.o camera.o simple_renderer.o 3d_model.o font.o
	gcc $(CFLGAS) -o demo matrix.o main.o display.o vector.o camera.o \
	                      simple_renderer.o 3d_model.o font.o -lSDL2

clean:
	rm main.o display.o matrix.o vector.o camera.o simple_renderer.o 3d_model.o font.o demo

main.o:		main.c polygon.h
	gcc $(CFLAGS) -o main.o -c main.c

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
