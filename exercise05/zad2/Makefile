shell := /bin/bash 
CFLAGS = -Wall  -pedantic 
CC = gcc
CFILES = $(wildcard *.c)
COBJS = $(CFILES:.c=.o)
EXEC = $(CFILES:.c=.exe)

.PHONY: all clean report

all: $(EXEC)

%.exe: %.o
	$(CC) $(CFLAGS) -o $@ $<

%.o: %.c
	$(CC) $(CFLAGS) -c $<

report: clean all
	echo "---------------------" > report.txt
	./integral.exe 0.001 1 >> report.txt
	@echo "---------------------" >> report.txt

	./integral.exe 0.001 10 >> report.txt
	@echo "---------------------" >> report.txt

	./integral.exe 0.001 100 >> report.txt
	
	@echo "---------------------" >> report.txt
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
	rm -f $(COBJS) $(EXEC)

