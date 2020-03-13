CC = gcc
CFLAGS = -Wall -Wextra -g -std=c89

.PHONY: all clean

all: so-cpp

so-cpp: so-cpp.o utils.o hashmap.o list.o
	$(CC) $(CFLAGS) $^ -o $@

so-cpp.o: so-cpp.c

utils.o: utils.c utils.h

hashmap.o: hashmap.c hashmap.h

list.o: list.c list.h

clean:
	-rm -f *.o so-cpp