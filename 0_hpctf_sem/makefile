# automatically generated makefile by ../scripts/create-make

CFLAGS=-Wall -g -O2 -std=gnu99 -I. -I./include -L./lib

LIBS=-lcitsky -lm -lpthread -lzmq -lczmq


all: client server

clean:
	rm -f  client server lib/error.o lib/lib.o lib/command.o lib/field.o lib/gamehelper.o lib/game.o lib/errorlog.o

client: client.c lib/libcitsky.a
	gcc $(CFLAGS) client.c $(LIBS) -o client

server: server.c lib/libcitsky.a
	gcc $(CFLAGS) server.c $(LIBS) -o server

lib/error.o: lib/error.c
	gcc -c $(CFLAGS) lib/error.c -o lib/error.o

lib/lib.o: lib/lib.c
	gcc -c $(CFLAGS) lib/lib.c -o lib/lib.o

lib/command.o: lib/command.c
	gcc -c $(CFLAGS) lib/command.c -o lib/command.o

lib/field.o: lib/field.c
	gcc -c $(CFLAGS) lib/field.c -o lib/field.o

lib/gamehelper.o: lib/gamehelper.c
	gcc -c $(CFLAGS) lib/gamehelper.c -o lib/gamehelper.o

lib/game.o: lib/game.c
	gcc -c $(CFLAGS) lib/game.c -o lib/game.o

lib/errorlog.o: lib/errorlog.c
	gcc -c $(CFLAGS) lib/errorlog.c -o lib/errorlog.o



lib/libcitsky.a: lib/error.o lib/lib.o lib/command.o lib/field.o lib/gamehelper.o lib/game.o lib/errorlog.o
	ar crs lib/libcitsky.a lib/error.o lib/lib.o lib/command.o lib/field.o lib/gamehelper.o lib/game.o lib/errorlog.o
