#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "hall_queue.h"
#include "semaphore.h"
#include "shared_memory.h"
#include "simulation_config.h"

static semaphore_t sem_queue;
static semaphore_t sem_chairs;
static semaphore_t sem_barbers;
static semaphore_t sem_clients;

void open_semaphores() {
  sem_queue = semaphore_open(SEMAPHORE_HALL_NAME);
  sem_chairs = semaphore_open(SEMAPHORE_CHAIRS_NAME);
  sem_barbers = semaphore_open(SEMAPHORE_BARBERS_NAME);
  sem_clients = semaphore_open(SEMAPHORE_CLIENTS_NAME);
}

char random_client() { return (char)(rand() % 100); }

int main(int argc, char **argv) {
  if (argc != 1) {
    printf("Usage: %s \n", argv[0]);
    return EXIT_FAILURE;
  }

  srand(time(NULL) + getpid());

  char *queue = shared_memory_create(IDENTIFICATOR, BUFFOR_SIZE);
  if (queue == NULL) {
    exit(EXIT_FAILURE);
  }

  open_semaphores();

  if (strlen(queue) >= HALL_PLACES_MAX_AMOUNT) {
    fprintf(stderr, "%s[ERROR]%s Queue is full\n", RED, RESET);
    exit(EXIT_FAILURE);
  }

  semaphore_wait(sem_queue);

  semaphore_wait(sem_clients);

  char client = random_client();
  printf("\t\t%s[Client]%s Client with id: %d entered the queue\n", YELLOW,
         RESET, client);
  fflush(stdout);
  queue_push(queue, client);
  semaphore_post(sem_clients);

  semaphore_post(sem_barbers);

  semaphore_wait(sem_chairs);

  printf("\t\t%s[Client]%s Client with id: %d done\n", GREEN, RESET, client);
  fflush(stdout);

  shared_memory_detach(queue);

  return EXIT_SUCCESS;
}