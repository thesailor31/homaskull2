all: lodepng.o main.o
	gcc *.o -o main

main.o: main.c lodepng.h
	gcc -c main.c

lodepng.o: lodepng.c lodepng.h
	gcc -c lodepng.c

clean:
	rm -f *.o
