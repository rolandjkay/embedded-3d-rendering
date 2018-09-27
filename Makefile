all: demo

run: demo
	./demo

demo:	main.o display.o matrix.o vector.o camera.o simple_renderer.o
	gcc -gdwarf-2 -o demo matrix.o main.o display.o vector.o camera.o simple_renderer.o -lSDL2

clean:
	rm main.o display.o matrix.o vector.o camera.o simple_renderer.o demo

main.o:		main.c polygon.h
	gcc -o main.o -c main.c

matrix.o:	matrix.c matrix.h
	gcc -o matrix.o -c matrix.c

camera.o:	camera.c camera.h
	gcc -o camera.o -c camera.c

vector.o:	vector.c vector.h
	gcc -o vector.o -c vector.c

display.o:	display.c display.h
	gcc -c display.c

simple_renderer.o:	simple_renderer.c simple_renderer.h
	gcc -c simple_renderer.c
