shell := /bin/bash
CC = gcc 
CFLAGS = -Wall -g -O3

.PHONY: all clean

all: server.c client.c utils.h
	$(CC) $(CFLAGS) server.c -o server.exe
	$(CC) $(CFLAGS) client.c -o client.exe

clean:
	rm -f *.exe *.txt
