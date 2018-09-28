all: demo

run: demo
	./demo

demo:	main.o display.o matrix.o vector.o camera.o simple_renderer.o 3d_model.o
	gcc -gdwarf-2 -o demo matrix.o main.o display.o vector.o camera.o \
	                      simple_renderer.o 3d_model.o -lSDL2

clean:
	rm main.o display.o matrix.o vector.o camera.o simple_renderer.o demo

main.o:		main.c polygon.h
	gcc -gdwarf-2 -o main.o -c main.c

matrix.o:	matrix.c matrix.h
	gcc -gdwarf-2 -o matrix.o -c matrix.c

camera.o:	camera.c camera.h
	gcc -gdwarf-2 -o camera.o -c camera.c

vector.o:	vector.c vector.h
	gcc -gdwarf-2 -o vector.o -c vector.c

display.o:	display.c display.h
	gcc -gdwarf-2 -c display.c

simple_renderer.o:	simple_renderer.c simple_renderer.h
	gcc -gdwarf-2 -o simple_renderer.o -c simple_renderer.c

3d_model.o:	3d_model.c 3d_model.h
	gcc -o 3d_model.o -c 3d_model.c
