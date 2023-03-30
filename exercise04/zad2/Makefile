shell := /bin/bash
CC = gcc
CFLAGS = -Wall -O3 -pedantic  
CFILES = $(wildcard *.c)


.PHONY: all clean main test

all : clean main

main: $(CFILES)
	$(CC) $(CFLAGS) -o $@.exe $^

test: clean main
	echo "Report jest kopią outputu w pliku 'output.txt' z programu ponieważ flagi UNIXA nie działają z Makefilem."

clean:
	rm -f *.o main.exe
