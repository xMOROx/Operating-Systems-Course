#include <semaphore.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "hall_queue.h"
#include "semaphore.h"
#include "shared_memory.h"
#include "simulation_config.h"

#define HAIRCUT_TIME 10000
#define TIMEOUT 100000

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

int main(int argc, char **argv) {
  if (argc != 1) {
    printf("Usage: %s \n", argv[0]);
    return EXIT_FAILURE;
  }

  char *queue = shared_memory_create(IDENTIFICATOR, BUFFOR_SIZE);
  if (queue == NULL) {
    exit(EXIT_FAILURE);
  }

  open_semaphores();

  printf("\t%s[Barber]%s Barber with id: %d started working\n", BLUE, RESET,
         getpid());
  fflush(stdout);

  while (true) {
    semaphore_wait(sem_barbers);

    semaphore_post(sem_clients);
    char haircut = queue_pop(queue);
    semaphore_post(sem_clients);

    printf("\t%s[Barber]%s Barber with id: %d started haircutting client with "
           "id: %d\n",
           CYAN, RESET, getpid(), haircut);
    fflush(stdout);

    usleep(HAIRCUT_TIME);

    printf("\t%s[Barber]%s Barber with id: %d finished haircutting client with "
           "id: %d\n",
           GREEN, RESET, getpid(), haircut);
    fflush(stdout);

    semaphore_post(sem_chairs);
    semaphore_post(sem_queue);

    if (queue_is_empty(queue)) {
      usleep(TIMEOUT);
      if (queue_is_empty(queue)) {
        break;
      }
    }
  }
  printf("\t%s[Barber]%s Barber with id: %d finished working\n", MAGENTA, RESET,
         getpid());
  fflush(stdout);

  shared_memory_detach(queue);
  return EXIT_SUCCESS;
}