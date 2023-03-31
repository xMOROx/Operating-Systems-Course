shell := /bin/bash 
CC = gcc
CFLAGS = -Wall -pedantic -g

.PHONY : all build clean integral prog_to_integrate

all: clean build

build: 
	@echo "Building..."
	@make integral
	@make prog_to_integrate

integral: integral.c
	$(CC) $(CFLAGS) -o $@.exe $^

prog_to_integrate: prog_to_integrate.c
	$(CC) $(CFLAGS) -o $@.exe $^

report: clean build
	@echo "Generating report..."
	@echo "---------------------" > report.txt
	./integral.exe 0.00000001 1 >> report.txt
	@echo "---------------------" >> report.txt

	./integral.exe 0.00000001 10 >> report.txt
	@echo "---------------------" >> report.txt

	./integral.exe 0.00000001 100 >> report.txt
	@echo "---------------------" >> report.txt

	./integral.exe 0.000000001 1 >> report.txt
	@echo "---------------------" >> report.txt

	./integral.exe 0.000000001 10 >> report.txt
	@echo "---------------------" >> report.txt

	./integral.exe 0.000000001 100 >> report.txt
	@echo "---------------------" >> report.txt

	./integral.exe 0.0000000001 1 >> report.txt
	@echo "---------------------" >> report.txt

	./integral.exe 0.0000000001 10 >> report.txt
	@echo "---------------------" >> report.txt

	./integral.exe 0.0000000001 100 >> report.txt
	@echo "---------------------" >> report.txt

clean:
	@echo "Cleaning..."
	@rm -f *.exe