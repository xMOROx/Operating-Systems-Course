shell := /bin/bash
CC = gcc
CFLAGS = -Wall -g -O3
CFILES = $(wildcard *.c)

all: $(CFILES:.c=.exe)

%.exe: %.c
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f *.exe
	

