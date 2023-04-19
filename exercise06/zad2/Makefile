shell := /bin/bash
CC = gcc 
CFLAGS = -Wall -g -O3

.PHONY: all clean

all: server.c client.c utils.h
	$(CC) $(CFLAGS) server.c -o server.exe -lrt
	$(CC) $(CFLAGS) client.c -o client.exe -lrt

clean:
	rm -f *.exe *.txt
