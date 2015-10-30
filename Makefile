.PHONY: all test valgrind clean
CFLAGS = -g
VALGGRINDFLAGS = --leak-check=full --show-leak-kinds=all

all: bdsm

test: valgrind

valgrind: unittest bdsm test.txt
	valgrind $(VALGGRINDFLAGS) ./unittest
	valgrind $(VALGGRINDFLAGS) ./bdsm < test.txt

clean:
	$(RM) bdsm unittest
