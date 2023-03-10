#include <stdio.h>

#ifdef BLOCK
#define BLOCK_SIZE 1024 // 1kB
#else
#define BLOCK_SIZE 1
#endif

enum Errors {
  WRONG_ARGUMENTS = -1,
  WRONG_ARGUMENTS_NUMBER = -2,
  SOURCE_FILE_NOT_FOUND = -3,
  DESTINATION_FILE_NOT_CREATE = -4,
  SOURCE_FILE_NOT_OPENED = -5,
  SOURCE_FILE_NOT_CLOSED = -6,
  WRONG_FIRST_CHAR = -7,
  WRONG_SECOND_CHAR = -8,

};

union Universal_File {
  FILE *file_lib;
  int file_sys;
};

struct Arguments {
  char *source_file_name;
  char *destination_file_name;
  char old_char;
  char new_char;
};

int change(struct Arguments *arguments);
void change_char_in_buffer(char **buffer, const char old_char,
                           const char new_char);
int check_arguments(int argc, char *argv[]);
int get_size_of_file(union Universal_File *file);
int Universal_open_file(union Universal_File *file, char *file_name);
int Universal_create_file(union Universal_File *file, char *file_name);
int Universal_read_file(union Universal_File *file, char **buffer);
int Universal_write_file(union Universal_File *file, char *buffer);
int Universal_close_file(union Universal_File *file);