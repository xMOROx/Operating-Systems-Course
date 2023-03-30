#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#define SIGNAL SIGUSR1
#define MAX(a, b) (((a) > (b)) ? (a) : (b))

typedef enum {
  NUMBERS = 1,
  CURRENT_TIME = 2,
  REQUEST_NUMBER = 3,
  CURRENT_TIME_PERIODICALLY = 4,
  EXIT = 5,
} STATES;

int request_received = 0;
int done = true;
int display_time_periodically = false;
int _;
STATES state = CURRENT_TIME;

void numbers(int n) {
  for (int i = 1; i <= n; i++) {
    printf("%d \n", i);
  }
}

void print_time() {
  time_t t = time(NULL);
  struct tm tm = *localtime(&t);

  if (tm.tm_sec < 10 && tm.tm_min < 10)
    printf("%d:0%d:0%d \n", tm.tm_hour, tm.tm_min, tm.tm_sec);
  else if (tm.tm_sec < 10)
    printf("%d:%d:0%d \n", tm.tm_hour, tm.tm_min, tm.tm_sec);
  else if (tm.tm_min < 10)
    printf("%d:0%d:%d \n", tm.tm_hour, tm.tm_min, tm.tm_sec);
  else
    printf("%d:%d:%d \n", tm.tm_hour, tm.tm_min, tm.tm_sec);
}

void current_time() { print_time(); }

void current_time_periodically() {
  while (display_time_periodically) {
    print_time();
    sleep(1);
  }
}

void requests() { printf("Valid requests: %d\n", request_received); }

void exit_catcher() {
  printf("Catcher exiting \n");
  fflush(NULL);
  exit(EXIT_SUCCESS);
}

void handle_signal(int sig, siginfo_t *info, void *ucontext) {
  pid_t sender_pid = info->si_pid;
  int request = info->si_status;

  printf("Received signal %d from process %d with value %d \n", sig, sender_pid,
         request);

  if (1 > request || request > 5) {
    printf("Invalid request %d\n", request);
  } else {
    request_received++;
    state = (STATES)request;
    done = false;
    display_time_periodically = (state == CURRENT_TIME_PERIODICALLY);
  }

  if (!display_time_periodically) {
    display_time_periodically = false;
  }

  kill(sender_pid, SIGNAL);
}

int main(int argc, char *argv[]) {
  if (argc != 1) {
    printf("Usage: %s <no arguments>", argv[0]);
    exit(EXIT_FAILURE);
  }

  struct sigaction si;
  si.sa_sigaction = handle_signal;
  sigemptyset(&si.sa_mask);
  si.sa_flags = SA_SIGINFO;
  sigaction(SIGNAL, &si, NULL);

  printf("Catcher starting \n");
  printf("Catcher PID to communicate with sender: %d \n", getpid());

  while (true) {
    _ = clock();

    if (done) {
      continue;
    }

    switch (state) {

    case NUMBERS: {
      printf("Catcher printing numbers \n");
      numbers(100);
      break;
    }

    case CURRENT_TIME: {

      printf("Catcher printing current time \n");

      current_time();

      break;
    }

    case CURRENT_TIME_PERIODICALLY: {

      current_time_periodically();

      break;
    }

    case REQUEST_NUMBER: {
      printf("Catcher printing number of requests \n");

      requests();

      break;
    }

    case EXIT: {
      exit_catcher();
      break;
    }
    }
    done = true;
    _ = clock();
  }
  return EXIT_SUCCESS;
}
