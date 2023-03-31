#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

#define MAX_CALLS 5

int call_id;
int depth;
typedef void (*handler_func)(int, siginfo_t *, void *);

void SIGINFO_handler(int sig, siginfo_t *info, void *ucontext) {
  printf("Signal number: %d \n", info->si_signo);
  printf("Signal code: %d \n", info->si_code);
  printf("PID: %d \n", info->si_pid);
  printf("UID: %d \n", info->si_uid);
  printf("Exit value: %x \n", info->si_status);
  printf("Value: %d \n", info->si_value.sival_int);
  printf("POSIX timer: %d \n", info->si_timerid);
}

void SA_NODEFER_handler(int sig, siginfo_t *info, void *ucontext) {
  printf("SA_NODEFER call id: %d, depth: %d\n", call_id, depth);
  call_id++;
  depth++;
  if (call_id < MAX_CALLS) {
    raise(SIGUSR1);
  }
  depth--;
  printf("SA_NODEFER call id: %d, depth: %d\n", call_id - 1, depth);
}

void SA_ONSTACK_handler(int sig, siginfo_t *info, void *ucontext) {
  printf("Signal number: %d \n", info->si_signo);
  printf("Signal code: %d \n", info->si_code);
  printf("PID: %d \n", info->si_pid);
  printf("UID: %d \n", info->si_uid);
  printf("Stack pointer: %p \n", ucontext);
  printf("Stack base: %p \n", ((ucontext_t *)ucontext)->uc_stack.ss_sp);
  printf("Stack size: %ld \n", ((ucontext_t *)ucontext)->uc_stack.ss_size);
  printf("Stack flags: %d \n", ((ucontext_t *)ucontext)->uc_stack.ss_flags);
  printf("Value: %d \n", info->si_value.sival_int);
}

void prepare_action(struct sigaction *saction, handler_func handler,
                    int flags) {
  saction->sa_sigaction = handler;
  saction->sa_flags = flags;
  sigemptyset(&saction->sa_mask);
  sigaction(SIGUSR1, saction, NULL);
  call_id = 0;
  depth = 0;
}

void SIGINFO_function(struct sigaction saction) {
  printf("\nSIGINFO function parent \n");

  prepare_action(&saction, SIGINFO_handler, SA_SIGINFO);

  printf("\nSIGINFO function child \n");

  if (fork() == 0) {
    prepare_action(&saction, SIGINFO_handler, SA_SIGINFO);
    kill(getpid(), SIGUSR1); // raise
    exit(EXIT_SUCCESS);
  } else {
    wait(NULL);
  }

  printf("\nSIGINFO custom \n");
  prepare_action(&saction, SIGINFO_handler, SA_SIGINFO);
  sigval_t value = {.sival_int = 0xAFFE};
  sigqueue(getpid(), SIGUSR1, value);
}

void SA_NODEFER_function(struct sigaction saction, int flags) {
  printf("\nSA_NODEFER function \n");
  prepare_action(&saction, SA_NODEFER_handler, flags);
  raise(SIGUSR1);
}

void SA_ONSTACK_function(struct sigaction saction, int flags) {
  printf("\nSA_ONSTACK function \n");

  stack_t stack;
  stack.ss_sp = malloc(SIGSTKSZ);
  stack.ss_size = SIGSTKSZ;
  stack.ss_flags = 0;
  prepare_action(&saction, SA_ONSTACK_handler, flags);

  printf("\nSA_ONSTACK function child \n");
  if (fork() == 0) {
    prepare_action(&saction, SA_ONSTACK_handler, flags);
    kill(getpid(), SIGUSR1);
    exit(EXIT_SUCCESS);
  } else {
    wait(NULL);
  }

  if (sigaltstack(&stack, NULL) == -1) {
    perror("sigaltstack");
    exit(EXIT_FAILURE);
  }

  printf("\nSA_ONSTACK Sending signal to self... \n");
  prepare_action(&saction, SA_ONSTACK_handler, flags);
  union sigval value;
  value.sival_int = 0xAFF;
  sigqueue(getpid(), SIGUSR1, value);
}

int main(int argc, char *argv[]) {
  if (argc != 1) {
    fprintf(stderr, "[Usage]: %s \n", argv[0]);
    exit(EXIT_FAILURE);
  }

  struct sigaction sigaction;

  printf("Testing SIGINFO \n");
  SIGINFO_function(sigaction);
  printf("\n");

  printf("Testing SA_NODEFER \n");
  SA_NODEFER_function(sigaction, SA_SIGINFO | SA_NODEFER);
  printf("\n");

  printf("Testing without SA_NODEFER  \n");
  SA_NODEFER_function(sigaction, SA_SIGINFO);
  printf("\n");

  printf("Testing SA_ONSTACK \n");
  SA_ONSTACK_function(sigaction, SA_SIGINFO | SA_ONSTACK);
  printf("\n");

  printf("Testing without SA_ONSTACK \n");
  SA_ONSTACK_function(sigaction, SA_SIGINFO);
  printf("\n");

  return EXIT_SUCCESS;
}