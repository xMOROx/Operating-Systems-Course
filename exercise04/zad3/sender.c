#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define SIGNAL SIGUSR1
#define START_ARG_NUM 2

volatile int request_confirmed = START_ARG_NUM;

void handle_signal(int sig) { request_confirmed++; }

int main(int argc, char *argv[]) {
  if (argc < 3) {
    printf("Usage: %s <pid> <request> \n", argv[0]);
    exit(EXIT_FAILURE);
  }

  int catcher_pid = atoi(argv[1]);
  int failed;
  int current_state;

  for (int i = START_ARG_NUM; i < argc; i++) {
    current_state = atoi(argv[i]);
    failed = false;

    struct sigaction action;

    sigemptyset(&action.sa_mask);
    action.sa_handler = handle_signal;
    sigaction(SIGNAL, &action, NULL);

    sigval_t value = {current_state};

    sigqueue(catcher_pid, SIGNAL, value);

    printf("Sent signal %d to process %d with value %d \n", SIGNAL, catcher_pid,
           current_state);

    time_t start = clock();
    while (request_confirmed <= i) {
      if (clock() - start > CLOCKS_PER_SEC * 3) {
        printf("No confirmation, retrying...\n");
        failed = true;
        i--;
        break;
      }
    }

    if (!failed) {
      printf("Request %d confirmed. \n", current_state);
    }
  }
  return EXIT_SUCCESS;
}
