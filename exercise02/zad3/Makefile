SHELL := /bin/bash
CC= gcc
CFLAGS= -Wall -O3
TEST_DIR = .
FILE_NAME = test
C_FILE = view_dir.c
all: build 

build: $(C_FILE) $(C_FILE:.c=.h)
	$(CC) $(CFLAGS) $< -o $(C_FILE:.c=.exe) 
test: clean build
	sudo ./$(C_FILE:.c=.exe) $(TEST_DIR) > $(FILE_NAME).txt
	tail -n 1 $(FILE_NAME).txt > $(FILE_NAME)2.txt
	cat $(FILE_NAME)2.txt > result.txt
	ls -r | grep $(FILE_NAME) | xargs rm -f

clean:
	rm -f *.exe *.txt
