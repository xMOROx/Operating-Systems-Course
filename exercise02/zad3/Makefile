SHELL := /bin/bash
CC= gcc
CFLAGS= -Wall -O3
TEST_DIR = .
FILE_NAME = test

all: build 

build: view_dir.c view_dir.h
	$(CC) $(CFLAGS) $< -o view_tree_recurently.exe

test: clean build
	sudo ./view_tree_recurently.exe $(TEST_DIR) > $(FILE_NAME).txt
	tail -n 1 $(FILE_NAME).txt > $(FILE_NAME)2.txt
	cat $(FILE_NAME)2.txt > result.txt
	ls -r | grep $(FILE_NAME) | xargs rm -f

clean:
	rm -f *.exe *.txt