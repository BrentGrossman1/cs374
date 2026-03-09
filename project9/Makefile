CC=gcc
CCOPTS=-Wall -Wextra -Werror
LIBS=-pthread

all: pc

pc: pc.c eventbuf.c
	$(CC) $(CCOPTS) -o pc pc.c eventbuf.c $(LIBS)

.PHONY: all clean

clean:
	rm -f pc