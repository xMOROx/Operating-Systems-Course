#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int check_if_digit(char *str) {
  int i = 0;
  while (str[i] != '\0') {
    if (str[i] < '0' || str[i] > '9') {
      return 0;
    }
    i++;
  }
  return 1;
}

int main(int argc, char *argv[]) {
  if (argc != 2) {
    printf("Usage: %s <number of children> \n", argv[0]);
    return 1;
  }
  if (!check_if_digit(argv[1])) {
    printf("The argument must be a number \n");
    return 2;
  }
  int n = atoi(argv[1]);
  pid_t pid;
  int i;
  for (i = 0; i < n; i++) {
    pid = fork();
    if (pid == -1) {
      printf("Error at fork()!    \n");
      return 3;
    }
    if (pid == 0) {
      printf("I am child with PID %d and my parent has PID %d \n", getpid(),
             getppid());
      return 0;
    }
  }
  for (i = 0; i < n; i++) {
    wait(NULL);
  }
  printf("I have %d children \n", n);
  return 0;
}
