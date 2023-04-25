#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include "hall_queue.h"
#include "semaphore.h"
#include "shared_memory.h"
#include "simulation_config.h"

#ifdef POSIX
#define BARBER_RUN "./barber_POSIX.exe"
#define CLIENT_RUN "./client_POSIX.exe"
#endif
#ifdef SYSV
#define BARBER_RUN "./barber_SYSV.exe"
#define CLIENT_RUN "./client_SYSV.exe"
#endif

static semaphore_t sem_queue;
static semaphore_t sem_chairs;
static semaphore_t sem_barbers;
static semaphore_t sem_clients;

void close_semaphores() {
  semaphore_close(sem_queue);
  semaphore_close(sem_chairs);
  semaphore_close(sem_barbers);
  semaphore_close(sem_clients);
}
void remove_semaphores() {
  semaphore_remove(SEMAPHORE_HALL_NAME);
  semaphore_remove(SEMAPHORE_CHAIRS_NAME);
  semaphore_remove(SEMAPHORE_BARBERS_NAME);
  semaphore_remove(SEMAPHORE_CLIENTS_NAME);
}
void create_semaphores() {
  sem_queue = semaphore_create(SEMAPHORE_HALL_NAME, CHAIR_MAX_AMOUNT);
  sem_chairs = semaphore_create(SEMAPHORE_CHAIRS_NAME, 0);
  sem_barbers = semaphore_create(SEMAPHORE_BARBERS_NAME, 0);
  sem_clients = semaphore_create(SEMAPHORE_CLIENTS_NAME, 1);
}

int main(int argc, char **argv) {
  if (argc != 1) {
    printf("Usage: %s \n", argv[0]);
    return EXIT_FAILURE;
  }

  printf("%s%s[Simulation]%s Simulation started\n", BOLD, BLUE, RESET);
  printf("\t Barbers total: %d\n", BARBER_MAX_AMOUNT);
  printf("\t Chairs total: %d\n", CHAIR_MAX_AMOUNT);
  printf("\t Clients total: %d\n", CLIENTS_AMOUNT);
  printf("\t Queue size: %d\n", HALL_PLACES_MAX_AMOUNT);

  fflush(stdout);

  char *shared = shared_memory_create(IDENTIFICATOR, BUFFOR_SIZE);
  if (shared == NULL) {
    exit(EXIT_FAILURE);
  }

  shared[0] = '\0';

  remove_semaphores();
  create_semaphores();

  for (int i = 0; i < BARBER_MAX_AMOUNT; i++)
    if (fork() == 0)
      execl(BARBER_RUN, BARBER_RUN, NULL);

  printf("%s%s[Simulation]%s Barbers created\n", BOLD, GREEN, RESET);
  fflush(stdout);

  for (int i = 0; i < CLIENTS_AMOUNT; i++)
    if (fork() == 0)
      execl(CLIENT_RUN, CLIENT_RUN, NULL);

  printf("%s%s[Simulation]%s Clients created\n", BOLD, GREEN, RESET);
  fflush(stdout);

  while (wait(NULL) > 0) {
  }

  if (!shared_memory_destroy(IDENTIFICATOR)) {
    fprintf(stderr, "%s%s[ERROR]%s Cannot destroy shared memory\n", BOLD, RED,
            RESET);
    exit(EXIT_FAILURE);
  }

  close_semaphores();
  printf("%s%s[Simulation]%s Simulation ended\n", BOLD, MAGENTA, RESET);
  fflush(stdout);
  return EXIT_SUCCESS;
}