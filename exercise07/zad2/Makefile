SHELL := /bin/bash
CC=gcc
CFLAGS=-Wall -O3 -g
OTHER=semaphore.c shared_memory.c hall_queue.c
OUTPUT = build/
all: 
		make clean
		mkdir $(OUTPUT)
		make simulation STANDARD=POSIX

simulation: client barber
	$(CC) $(CFLAGS) $(SOURCES) start_simulation.c $(OTHER) -D$(STANDARD) -lrt -lpthread -o $(OUTPUT)start_simulation_$(STANDARD).exe

client:
	$(CC) $(CFLAGS) $(SOURCES) client.c $(OTHER) -D$(STANDARD) -lrt -lpthread -o $(OUTPUT)client_$(STANDARD).exe

barber:
	$(CC) $(CFLAGS) $(SOURCES) barber.c $(OTHER) -D$(STANDARD) -lrt -lpthread -o $(OUTPUT)barber_$(STANDARD).exe

clean:
	rm -rf $(OUTPUT)
