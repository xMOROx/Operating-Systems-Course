#include "signal_testing.h"
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define USAGE "Usage: %s {ignore|mask|pending} \n"
int parse_first_argument(char *arg, Mode *mode) {
  for (int i = 0; i < 4; i++) {
    if (!strcmp(arg, "handler"))
      return EXIT_FAILURE;

    if (strcmp(arg, mode_str[i]) == 0) {
      *mode = i;
      return EXIT_SUCCESS;
    }
  }
  return EXIT_FAILURE;
}

int main(int argc, char *argv[]) {
  if (argc != 2) {
    fprintf(stderr, USAGE, argv[0]);
    exit(EXIT_FAILURE);
  }

  Mode mode;

  if (parse_first_argument(argv[1], &mode) == EXIT_FAILURE) {
    fprintf(stderr, USAGE, argv[0]);
    exit(EXIT_FAILURE);
  }

  if (mode == IGNORE) {
    signal(SIGUSR1, SIG_IGN);
  }

  if (mode == PENDING) {
    raise(SIGUSR1);
  }

  if (mode == PENDING || mode == MASK) {
    sigset_t mask;
    sigpending(&mask);
    if (sigismember(&mask, SIGUSR1)) {
      fprintf(stdout, "Signal is pending in child \n");
    } else {
      fprintf(stdout, "Signal is not pending in child \n");
    }
  }

  return 0;
}