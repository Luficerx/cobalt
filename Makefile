CC = gcc
PROG_NAME = cobalt
PROG = bin/$(PROG_NAME)

CFLAGS_EXTRA = -Wall -Wextra -g -O3
CFLAGS = -I./include -L./lib/ $(CFLAGS_EXTRA)

SRC = $(wildcard ./src/*.c)

$(PROG): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(PROG)

all: clean $(PROG)

clean:
	rm -f $(PROG)