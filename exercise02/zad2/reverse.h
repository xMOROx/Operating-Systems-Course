#include <stdio.h>
#define MAX_FILE_NAME 1024

enum ERROR {
  NO_ERROR = 0,
  WRONG_ARGUMENTS_NUMBER = -1,
  WRONG_FILE_CLOSE = -2,
  FILE_EMPTY = -3,
  FILE_NOT_EXIST = -4,
  FILE_NOT_OPEN = -5,
  FILE_NOT_CREATED = -6,
  FILE_NOT_COPIED = -7,
  FILE_NOT_REVERSED = -8,
  FILE_NOT_CLOSED = -9,
  FILE_NAME_TOO_LONG = -10,
  FILE_ALREADY_EXIST = -11,
};

int check_arguments(int argc, char *argv[]);
int open_file(char *file_name, FILE **file);
int close_file(FILE *file);
int reverse_file(char *source_file_name, char *destination_file_name);
int create_file(char *file_name, FILE **file);
char *reverse_string(char *string);
int copy_file(FILE *file, FILE *new_file);
