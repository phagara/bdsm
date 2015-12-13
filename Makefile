.PHONY: all test valgrind clean
CFLAGS = -g --pedantic -Wextra -Wall -Wfloat-equal -Wundef -Wshadow -Wpointer-arith \
		 -Wcast-align -Wstrict-prototypes -Wstrict-overflow=5 -Wwrite-strings \
		 -Waggregate-return -Wcast-qual -Wswitch-default -Wswitch-enum -Wconversion \
		 -Wunreachable-code -Wformat=2 -Winit-self -Wmissing-prototypes -Os \
		 -Werror -Werror-implicit-function-declaration
VALGGRINDFLAGS = --leak-check=full --show-leak-kinds=all

all: bdsm

test: valgrind

clean:
	$(RM) *.o bdsm unittest

bdsm: bdsm.o buffer.o bookstore.o
	$(CC) $(CFLAGS) -o bdsm bdsm.o buffer.o bookstore.o

unittest: unittest.o buffer.o bookstore.o
	$(CC) $(CFLAGS) -o unittest unittest.o buffer.o bookstore.o

valgrind: unittest bdsm test.txt
	valgrind $(VALGGRINDFLAGS) ./unittest
	valgrind $(VALGGRINDFLAGS) ./bdsm < test.txt
