SHELL := /bin/bash
CC=gcc
CFLAGS=-Wall -O3 -g
OTHER=semaphore.c shared_memory.c hall_queue.c

all: 
		make simulation STANDARD=POSIX
		make simulation STANDARD=SYSV

simulation: client barber
	$(CC) $(CFLAGS) $(SOURCES) start_simulation.c $(OTHER) -D$(STANDARD) -lrt -lpthread -o start_simulation_$(STANDARD).exe

client:
	$(CC) $(CFLAGS) $(SOURCES) client.c $(OTHER) -D$(STANDARD) -lrt -lpthread -o client_$(STANDARD).exe

barber:
	$(CC) $(CFLAGS) $(SOURCES) barber.c $(OTHER) -D$(STANDARD) -lrt -lpthread -o barber_$(STANDARD).exe

clean:
	rm -rf barber_*.exe client_*.exe start_simulation_*.exe
