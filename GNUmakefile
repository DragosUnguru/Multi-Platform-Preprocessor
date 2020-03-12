CC = gcc
CFLAGS = -Wall -Wextra -g -std=c89

.PHONY: all clean

all: so-cpp

so-cpp: so-cpp.o utils.o hashmap.o list.o
	$(CC) $^ -o $@

so-cpp.o: so-cpp.c
	$(CC) $(CFLAGS) -c $<

utils.o: utils.c utils.h
	$(CC) $(CFLAGS) -c $<

hashmap.o: hashmap.c hashmap.h
	$(CC) $(CFLAGS) -c $<

list.o: list.c list.h
	$(CC) $(CFLAGS) -c $<

clean:
	-rm -f *.o so-cpp
