CC=gcc
CFLAGS=-g #-pedantic -std=gnu17 -Wall -Werror -Wextra

.PHONY: all
all: nyufile

nyufile: nyufile.o fat32disk.o directory.o
nyufile.o: fat32disk.c directory.c
fat32disk.o: fat32disk.c fat32disk.h
directory.o: directory.c directory.h

.PHONY: clean
clean:
	rm -f *.o nyufile
