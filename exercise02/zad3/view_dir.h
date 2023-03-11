#include <dirent.h>

#define NO_ERROR 0
enum ERRORS {
  FAIL_CLOSE_DIR = -1,
  FAIL_OPEN_DIR = -2,
  FAIL_GET_STAT = -3,
  WRONG_ARGUMENTS_NUMBER = -4,
};

DIR *open_dir(char *path);
int close_dir(DIR *dir);
int read_dir(char *path);
