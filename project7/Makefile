CC=gcc
CFLAGS=-Wall -Wextra -Werror -lpthread

grayscaler: grayscaler.c ppm.c ppm.h
	$(CC) $(CFLAGS) -o grayscaler grayscaler.c ppm.c

clean:
	rm -f grayscaler