all: main

main:
	gcc -Wall -O3 -o 2A03 -lm -lSDL main.c globals.c osc.c parser.c util.c -march=native -mfpmath=sse
	
mingw:
	i686-pc-mingw32-gcc -Wall -O3 -o 2A03.exe main.c globals.c osc.c parser.c util.c -march=native -mfpmath=sse `i686-pc-mingw32-pkg-config sdl --libs --cflags`
