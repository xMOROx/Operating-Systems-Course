#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int check_if_directory(char *path) {
  struct stat buffor;
  if (stat(path, &buffor) == -1) {
    perror("stat ");
    return -1;
  }
  return S_ISDIR(buffor.st_mode);
}

int main(int argc, char *argv[]) {
  if (argc != 2) {
    fprintf(stderr, "Usage: %s <path>\n", argv[0]);
    return 1;
  }

  if (check_if_directory(argv[1]) == 0) {
    fprintf(stderr, "%s is not a directory\n", argv[1]);
    return 1;
  }
  if (check_if_directory(argv[1]) == -1) {
    return 1;
  }

  printf("Name: %s ", argv[0]);

  setbuf(stdout, NULL); // Disable buffering of stdout

  if (execl("/usr/bin/ls", "ls", "-l", argv[1], NULL) == -1) {
    perror("execl");
    return 1;
  }
}