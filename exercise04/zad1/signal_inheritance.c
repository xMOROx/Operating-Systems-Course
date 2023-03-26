#include "signal_testing.h"
#include <bits/types/sigset_t.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <wait.h>

#define USAGE "Usage: %s {ignore|mask|handler|pending} {fork/exec} \n"
char action_str[2][5] = {"fork", "exec"};

void handler(int signum) {
  printf("Child %d received signal %d from parent %d \n", getpid(), signum,
         getppid());
}

int parse_first_argument(char *arg, Mode *mode) {
  for (int i = 0; i < 4; i++) {
    if (strcmp(arg, mode_str[i]) == 0) {
      *mode = i;
      return EXIT_SUCCESS;
    }
  }

  return EXIT_FAILURE;
}

int parse_second_argument(char *arg, Action *action) {
  for (int i = 0; i < 2; i++) {
    if (strcmp(arg, action_str[i]) == 0) {
      *action = i;
      return EXIT_SUCCESS;
    }
  }
  fprintf(stderr, "Invalid action \n");
  return EXIT_FAILURE;
}

int parse_arguments(char *arg[], Mode *mode, Action *action) {
  if (parse_first_argument(arg[1], mode) == EXIT_FAILURE) {
    return EXIT_FAILURE;
  }

  if (parse_second_argument(arg[2], action) == EXIT_FAILURE) {

    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int parse_mode(Mode mode) {
  switch (mode) {
  case IGNORE: {
    printf("Ignoring signal \n");
    signal(SIGUSR1, SIG_IGN);
    break;
  }
  case HANDLER: {
    printf("Handling signal \n");
    signal(SIGUSR1, handler);
    break;
  }
  case PENDING:
  case MASK: {
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGUSR1);

    if (mode == MASK) {
      printf("Masking signal \n");
    } else {
      printf("Pending signal \n");
    }

    if (sigprocmask(SIG_BLOCK, &mask, NULL) == -1) {
      fprintf(stderr, "Error setting mask \n");
      return EXIT_FAILURE;
    }

    break;
  }
  }

  return EXIT_SUCCESS;
}

int parse_action(Action action) {
  switch (action) {
  case FORK: {
    printf("Forking \n");
    break;
  }
  case EXEC: {
    printf("Executing \n");
    break;
  }
  }

  return EXIT_SUCCESS;
}

int main(int argc, char *argv[]) {
  if (argc != 3) {
    fprintf(stderr, USAGE, argv[0]);
    exit(EXIT_FAILURE);
  }

  Mode mode;
  Action action;

  if (parse_arguments(argv, &mode, &action) == EXIT_FAILURE) {
    fprintf(stderr, USAGE, argv[0]);
    exit(EXIT_FAILURE);
  }

  if (parse_mode(mode) == EXIT_FAILURE) {
    fprintf(stderr, "Error parsing mode \n");
    exit(EXIT_FAILURE);
  }

  if (parse_action(action) == EXIT_FAILURE) {
    fprintf(stderr, "Error parsing action \n");
    exit(EXIT_FAILURE);
  }

  sigset_t mask;

  raise(SIGUSR1);

  if (mode == MASK || mode == PENDING) {
    sigpending(&mask);
    if (sigismember(&mask, SIGUSR1)) {
      printf("Signal is pending in parent \n");
    }
  }

  if (action == FORK) {
    pid_t child_pid = fork();

    if (child_pid == -1) {
      fprintf(stderr, "Error creating child \n");
      exit(EXIT_FAILURE);
    }

    if (child_pid == 0) {
      if (mode != PENDING)
        raise(SIGUSR1);

      if (mode == MASK || mode == PENDING) {
        sigpending(&mask);
        if (sigismember(&mask, SIGUSR1)) {
          printf("Signal is pending in child \n");
        }
      }
    } else {
      printf("Signal not pending in child \n");
    }
  } else if (action == EXEC) {
    execl("./exec", "./exec", argv[1], NULL);
  }

  wait(NULL);
  return 0;
}
