CC = gcc
PROG_NAME = cobalt
PROG = bin/$(PROG_NAME)

CTFLAGS = -DTRACK_ARENA_ALLOCATION
CFLAGS_EXTRA = -Wall -Wextra -pedantic -g
CFLAGS = -I./include -L./lib/ $(CFLAGS_EXTRA) $(CTFLAGS)

SRC = $(wildcard ./src/*.c)

$(PROG): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(PROG)

all: clean $(PROG)

clean:
	rm -f $(PROG)