SHELL := /bin/bash
CC=gcc 
CFLAGS=	-g -Wall -Werror -Wextra -pedantic  -O3
FILE_NAME= ftw

all: $(FILE_NAME).c $(FILE_NAME).h
	$(CC) $(CFLAGS) -o $(FILE_NAME).exe $(FILE_NAME).c
	
test: clean all 
	./$(FILE_NAME).exe ~ aaa > result.txt


clean: 
		rm -rf *.exe *.txt
