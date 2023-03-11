SHELL := /bin/bash
CC=gcc
CFLAGS= -Wall -O3
NAME=change
BAJT=BAJT
SYS=SYS
LIB=LIB
FILE_TXT_NAME=pomiar

all: report

build: $(NAME).c $(NAME).h
	make $(NAME) SUFFIX_1=$(BAJT) SUFFIX_2=$(SYS)
	make $(NAME) SUFFIX_1=$(BAJT) SUFFIX_2=$(LIB)

$(NAME): $(NAME).c $(NAME).h
	$(CC) $(CFLAGS) -o $@_$(SUFFIX_1)_$(SUFFIX_2).exe $< -D $(SUFFIX_1) -D $(SUFFIX_2) 

reportTestPerformance: $(NAME)_$(SUFFIX_1)_$(SUFFIX_2).exe 
	head -c 1048576 </dev/urandom >test.in
	./$(NAME)_$(SUFFIX_1)_$(SUFFIX_2).exe a b test.in $(NAME)_$(SUFFIX_1)_$(SUFFIX_2).txt > $(FILE_TXT_NAME)_$(SUFFIX_1)_$(SUFFIX_2).txt
	tr a b < test.in > tmp.txt
	diff -s tmp.txt $(NAME)_$(SUFFIX_1)_$(SUFFIX_2).txt >> $(FILE_TXT_NAME)_$(SUFFIX_1)_$(SUFFIX_2).txt
	sed -i  '1 i\Time of $(SUFFIX_1) $(SUFFIX_2): ' $(FILE_TXT_NAME)_$(SUFFIX_1)_$(SUFFIX_2).txt
	rm -f test.in 

reportBuild: 
	make reportTestPerformance SUFFIX_1=$(BAJT) SUFFIX_2=$(SYS)
	make reportTestPerformance SUFFIX_1=$(BAJT) SUFFIX_2=$(LIB)

report: clean build reportBuild 

	cat $(FILE_TXT_NAME)_$(BAJT)_$(SYS).txt $(FILE_TXT_NAME)_$(BAJT)_$(LIB).txt > $(FILE_TXT_NAME)_zad_1.txt
	ls -r | grep -i .txt | xargs bash -c 'echo $$@' | xargs bash -c 'rm -f $$@'
	rm -f tmp.txt
clean: 
	rm -f *.exe *.txt *.in
