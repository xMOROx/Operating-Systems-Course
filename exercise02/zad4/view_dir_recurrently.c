#include <dirent.h>
#include <errno.h>
#include <ftw.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
int _;

#define NO_ERROR 0
enum ERRORS {
  FAIL_GET_STAT = -1,
  WRONG_ARGUMENTS_NUMBER = -2,
};

long long total_size = 0;

int file_view_callback(const char *fpath, const struct stat *sb, int typeflag) {
  if (typeflag != FTW_D) {
    struct stat buffor_stat;
    _ = stat(fpath, &buffor_stat);
    printf("File: %s %ld \n", fpath, buffor_stat.st_size);
    total_size += buffor_stat.st_size;
  }
  return NO_ERROR;
}

int main(int argc, char *argv[]) {
  if (argc != 2) {
    fprintf(stderr, "Wrong number of arguments. You need to pass 0! \n");
    return WRONG_ARGUMENTS_NUMBER;
  }
  if (ftw(argv[1], file_view_callback, 1) == -1) {
    fprintf(stderr, "Cannot open current directory! \n %s. \n",
            strerror(errno));
    return FAIL_GET_STAT;
  }
  printf("Size of all files: %lld \n", total_size);

  return NO_ERROR;
}