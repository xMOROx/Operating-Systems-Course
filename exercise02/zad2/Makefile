SHELL := /bin/bash
CC=gcc 
CFLAGS= -Wall -O3
NAME=reverse
SIZE_1=1
SIZE_2=1024
NAME_TEST=pomiar
EXT=.txt

all: report
build: $(NAME).c $(NAME).h
	$(CC) $(CFLAGS) -o $(NAME)_$(SIZE_1).exe $(NAME).c -D BLOCK_SIZE=$(SIZE_1)
	$(CC) $(CFLAGS) -o $(NAME)_$(SIZE_2).exe $(NAME).c -D BLOCK_SIZE=$(SIZE_2)

reportTestPerformance: $(NAME)_$(SIZE_1).exe $(NAME)_$(SIZE_1).exe
	head -c 1048576 </dev/urandom >test.in
	./$(NAME)_$(SIZE_1).exe test.in $(NAME)_$(SIZE_1)$(EXT) > $(NAME_TEST)_$(SIZE_1)_zad_2$(EXT)
	./$(NAME)_$(SIZE_2).exe test.in $(NAME)_$(SIZE_2)$(EXT) > $(NAME_TEST)_$(SIZE_2)_zad_2$(EXT)
	sed -i  '1 i\Time of $(SIZE_1): ' $(NAME_TEST)_$(SIZE_1)_zad_2$(EXT)
	sed -i  '1 i\Time of $(SIZE_2): ' $(NAME_TEST)_$(SIZE_2)_zad_2$(EXT)
	rm -f test.in

report: clean build reportTestPerformance
	cat $(NAME_TEST)_$(SIZE_1)_zad_2$(EXT) $(NAME_TEST)_$(SIZE_2)_zad_2$(EXT) > $(NAME_TEST)_zad_2$(EXT)
	ls -r | grep -i pomiar | xargs bash  -c 'rm -f $$@'
	rm -f $(NAME)_$(SIZE_1)$(EXT) $(NAME)_$(SIZE_2)$(EXT) 
clean:
	rm -f *.exe *$(EXT) *.in