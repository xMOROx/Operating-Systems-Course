#include "ftw.h"
#include <dirent.h>
#include <linux/limits.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

size_t check_arguments(char *argv[]) {
  struct stat st;

  if (strlen(argv[1]) > PATH_MAX) {
    perror("path too long");
    exit(EXIT_FAILURE);
  }

  if (lstat(argv[1], &st) == -1) {
    perror("lstat");
    exit(EXIT_FAILURE);
  }
  if (S_ISLNK(st.st_mode))
    exit(EXIT_SUCCESS); // ignore symbolic links

  if (stat(argv[1], &st) == -1) {
    perror("stat");
    exit(EXIT_FAILURE);
  }

  if (access(argv[1], R_OK) == -1) {
    perror("no read permission");
    exit(EXIT_FAILURE);
  }

  if (argv[1][strlen(argv[1]) - 1] == '/') {
    perror("path ends with /");
    exit(EXIT_FAILURE);
  }

  if (strlen(argv[2]) == 0) {
    perror("empty pattern");
    exit(EXIT_FAILURE);
  }

  int lenght = strlen(argv[2]);

  if (lenght > NAME_MAX) {
    perror("pattern too long");
    exit(EXIT_FAILURE);
  }

  return 0;
}

int main(int argc, char *argv[]) {
  if (argc != 3) {
    fprintf(stderr, "Usage: %s <path> <pattern> \n", argv[0]);
    exit(1);
  }

  if (check_arguments(argv)) {
    exit(EXIT_FAILURE);
  }

  DIR *dir;
  struct dirent *entry;
  struct stat st;
  char *pattern = argv[2];
  char current_path[PATH_MAX];
  char entry_path[PATH_MAX];
  char exec_path[PATH_MAX];
  int size_pattern = strlen(pattern);

  realpath(argv[0], exec_path);

  if (exec_path[0] != '/') {
    perror("exec path not absolute");
    exit(EXIT_FAILURE);
  }

  realpath(argv[1], current_path);

  if (stat(current_path, &st) == -1) {
    perror("stat current path");
    exit(EXIT_FAILURE);
  }

  if (S_ISDIR(st.st_mode)) {
    dir = opendir(current_path);
    if (dir == NULL) {
      perror("opendir");
      exit(EXIT_FAILURE);
    }
    while ((entry = readdir(dir))) {
      if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
        continue;

      strcpy(entry_path, current_path);
      strcat(entry_path, "/");
      strcat(entry_path, entry->d_name);

      if (fork() == 0) {
        execl(exec_path, exec_path, entry_path, pattern, NULL);
      }
    }

  } else {

    FILE *file = fopen(current_path, "r");
    char buffor[size_pattern];

    if (file == NULL) {
      perror("fopen");
      exit(EXIT_FAILURE);
    }

    size_t size = fread(buffor, sizeof(char), size_pattern, file);
    buffor[size] = '\0'; // add null terminator

    if (strcmp(buffor, pattern) == 0) {
      printf("PATH: %s PID: %d \n", current_path, getpid());
      setbuf(stdout, NULL);
    }
  }

  while (wait(NULL) > 0)
    ;
}