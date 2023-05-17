CC = gcc
CFLAGS = -Wall
LIBS = -lpthread
CFILES = $(wildcard *.c)

all: main 

main: $(CFILES) 
	$(CC) $(CFLAGS) $<  -o $(CFILES:.c=.exe)  $(LIBS)

test: main
	./$(CFILES:.c=.exe) > report.txt

clean:
	rm -f *.exe
