CC = gcc
PROG = bin/cobalt

CFLAGS_EXTRA = -Wall -Wextra -g
CFLAGS = -I./include -L./lib/ $(CFLAGS_EXTRA)

SRC = $(wildcard ./src/*.c)

all: clean $(PROG)

$(PROG): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(PROG)

clean:
	@rm -f $(PROG)

run: $(PROG)
	./$(PROG) $(ARGS)