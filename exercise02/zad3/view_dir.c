#include "view_dir.h"
#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
int _;

DIR *open_dir(char *path) {
  DIR *dir;
  // check if path is a directory
  struct stat buffor_stat;
  _ = stat(path, &buffor_stat);
  if (!S_ISDIR(buffor_stat.st_mode)) {
    fprintf(stderr, "Path is not a directory! \n %s. \n", strerror(errno));
    return NULL;
  }
  if ((dir = opendir(path)) == NULL) {
    fprintf(stderr, "Cannot open current directory! \n %s. \n",
            strerror(errno));
    return NULL;
  }
  return dir;
}

int close_dir(DIR *dir) {
  if (!closedir(dir)) {
    fprintf(stderr, "Cannot close current directory! \n %s. \n",
            strerror(errno));
    return FAIL_CLOSE_DIR;
  }
  return NO_ERROR;
}

int read_dir(char *path) {
  DIR *dir = open_dir(path);
  if (dir == NULL) {
    return FAIL_OPEN_DIR;
  }
  struct dirent *entry;
  struct stat buffor_stat;
  long long int size = 0;

  while ((entry = readdir(dir)) != NULL) {
    _ = stat(entry->d_name, &buffor_stat);
    if (S_ISDIR(buffor_stat.st_mode)) {
      continue;
    }

    size += buffor_stat.st_size;
    printf("File: %s %ld \n", entry->d_name, buffor_stat.st_size);
  }
  printf("Size of all files: %lld \n", size);

  return NO_ERROR;
}

int main(int argc, char *argv[]) {
  if (argc != 2) {
    fprintf(stderr, "Wrong number of arguments. You need to pass 0! \n");
    return WRONG_ARGUMENTS_NUMBER;
  }

  return read_dir(argv[1]);
}