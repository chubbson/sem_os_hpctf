# automatically generated makefile by ../scripts/create-make

CFLAGS=-Wall -g -O2 -std=gnu99 -I. -I./include -L./lib

LIBS=-lcitsky -lm -lpthread


all: server

clean:
	rm -f  server lib/error.o lib/lib.o lib/field.o lib/game.o lib/errorlog.o

server: server.c lib/libcitsky.a
	gcc $(CFLAGS) server.c $(LIBS) -o server

lib/error.o: lib/error.c
	gcc -c $(CFLAGS) lib/error.c -o lib/error.o

lib/lib.o: lib/lib.c
	gcc -c $(CFLAGS) lib/lib.c -o lib/lib.o

lib/field.o: lib/field.c
	gcc -c $(CFLAGS) lib/field.c -o lib/field.o

lib/game.o: lib/game.c
	gcc -c $(CFLAGS) lib/game.c -o lib/game.o

lib/errorlog.o: lib/errorlog.c
	gcc -c $(CFLAGS) lib/errorlog.c -o lib/errorlog.o



lib/libcitsky.a: lib/error.o lib/lib.o lib/field.o lib/game.o lib/errorlog.o
	ar crs lib/libcitsky.a lib/error.o lib/lib.o lib/field.o lib/game.o lib/errorlog.o
