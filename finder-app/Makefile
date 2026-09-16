CROSS_COMPILE ?=
CFLAGS = -g -Wall

ifeq ($(CROSS_COMPILE), aarch64-none-linux-gnu-)
	CC = $(CROSS_COMPILE)gcc
else
	CC = gcc
endif

all: writer

writer: writer.o
	$(CC) $(CFLAGS) writer.o -o writer

writer.o: writer.c
	$(CC) $(CFLAGS) -c -o writer.o writer.c

clean:
	rm -f *.o writer