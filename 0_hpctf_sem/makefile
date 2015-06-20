# automatically generated makefile by ../scripts/create-make

CFLAGS=-Wall -g -O2 -std=gnu99 -I. -I./include -L./lib

LIBS=-lcitsky -lm -lpthread -lzmq -lczmq


all: fldviewer client server

clean:
	rm -f  fldviewer client server lib/error.o lib/lib.o lib/svlb_helper.o lib/command.o lib/kvmaphelper.o lib/field.o lib/clistrategies.o lib/kvsimple.o lib/gamehelper.o lib/somecolor.o lib/game.o lib/errorlog.o lib/fieldhelper.o

fldviewer: fldviewer.c lib/libcitsky.a
	gcc $(CFLAGS) fldviewer.c $(LIBS) -o fldviewer

client: client.c lib/libcitsky.a
	gcc $(CFLAGS) client.c $(LIBS) -o client

server: server.c lib/libcitsky.a
	gcc $(CFLAGS) server.c $(LIBS) -o server

lib/error.o: lib/error.c
	gcc -c $(CFLAGS) lib/error.c -o lib/error.o

lib/lib.o: lib/lib.c
	gcc -c $(CFLAGS) lib/lib.c -o lib/lib.o

lib/svlb_helper.o: lib/svlb_helper.c
	gcc -c $(CFLAGS) lib/svlb_helper.c -o lib/svlb_helper.o

lib/command.o: lib/command.c
	gcc -c $(CFLAGS) lib/command.c -o lib/command.o

lib/kvmaphelper.o: lib/kvmaphelper.c
	gcc -c $(CFLAGS) lib/kvmaphelper.c -o lib/kvmaphelper.o

lib/field.o: lib/field.c
	gcc -c $(CFLAGS) lib/field.c -o lib/field.o

lib/clistrategies.o: lib/clistrategies.c
	gcc -c $(CFLAGS) lib/clistrategies.c -o lib/clistrategies.o

lib/kvsimple.o: lib/kvsimple.c
	gcc -c $(CFLAGS) lib/kvsimple.c -o lib/kvsimple.o

lib/gamehelper.o: lib/gamehelper.c
	gcc -c $(CFLAGS) lib/gamehelper.c -o lib/gamehelper.o

lib/somecolor.o: lib/somecolor.c
	gcc -c $(CFLAGS) lib/somecolor.c -o lib/somecolor.o

lib/game.o: lib/game.c
	gcc -c $(CFLAGS) lib/game.c -o lib/game.o

lib/errorlog.o: lib/errorlog.c
	gcc -c $(CFLAGS) lib/errorlog.c -o lib/errorlog.o

lib/fieldhelper.o: lib/fieldhelper.c
	gcc -c $(CFLAGS) lib/fieldhelper.c -o lib/fieldhelper.o



lib/libcitsky.a: lib/error.o lib/lib.o lib/svlb_helper.o lib/command.o lib/kvmaphelper.o lib/field.o lib/clistrategies.o lib/kvsimple.o lib/gamehelper.o lib/somecolor.o lib/game.o lib/errorlog.o lib/fieldhelper.o
	ar crs lib/libcitsky.a lib/error.o lib/lib.o lib/svlb_helper.o lib/command.o lib/kvmaphelper.o lib/field.o lib/clistrategies.o lib/kvsimple.o lib/gamehelper.o lib/somecolor.o lib/game.o lib/errorlog.o lib/fieldhelper.o
