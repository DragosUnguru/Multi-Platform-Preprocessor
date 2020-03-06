CC = gcc
CFLAGS = -Wall -Wextra -g -std=c89

.PHONY: all clean

all: so-cpp

so-cpp: so-cpp.o hashmap.o list.o

so-cpp.o: so-cpp.c

hashmap.o: hashmap.c hashmap.h

list.o: list.c list.h

clean:
	-rm -f *.o so-cpp
