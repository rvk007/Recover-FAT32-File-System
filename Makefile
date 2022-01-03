CC=gcc
CFLAGS=-g -pedantic -std=gnu99 -Wall -Wextra #-Werror

.PHONY: all
all: nyufile

run=nyufile.o fat32disk.o directory.o recover.o

nyufile: $(run)
	$(CC) $(CFLAGS) -o nyufile $(run) -l crypto
nyufile.o: fat32disk.c directory.c recover.c
fat32disk.o: fat32disk.c fat32disk.h
directory.o: directory.c directory.h
recover.o: recover.c recover.h directory.h

.PHONY: clean
clean:
	rm -f *.o nyufile
