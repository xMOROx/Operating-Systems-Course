SHELL := /bin/bash
CC = gcc
CFLAGS = -Wall -g -O3 -pedantic 

.PHONY: all clean clean_object catcher.o catcher sender.o sender
all: catcher sender clean_object

catcher.o: catcher.c
	$(CC) $(CFLAGS) -c -o $@ $^

catcher: catcher.o
	$(CC) $(CFLAGS) -o $@.exe $^

sender.o: sender.c
	$(CC) $(CFLAGS) -c -o $@ $^

sender: sender.o
	$(CC) $(CFLAGS) -o $@.exe $^

clean_object: 
	rm -r *.o

clean:
	rm -f *.o *.exe 
