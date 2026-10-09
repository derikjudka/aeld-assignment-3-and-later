CROSS_COMPILE ?=
CFLAGS = -g -Wall -Werror

ifeq ($(CROSS_COMPILE), aarch64-none-linux-gnu-)
	CC = $(CROSS_COMPILE)gcc
else
	CC = gcc
endif

all: aesdsocket

aesdsocket: aesdsocket.o
	$(CC) $(CFLAGS) aesdsocket.o -o aesdsocket

aesdsocket.o: aesdsocket.c
	$(CC) $(CFLAGS) -c -o aesdsocket.o aesdsocket.c

clean:
	rm -f *.o aesdsocket