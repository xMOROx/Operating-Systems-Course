#include "reverse.h"
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
int _; // to avoid unused variable warning

int check_arguments(int argc, char *argv[]) {
  if (argc != 3) {
    fprintf(stderr,
            "[Reversing] Wrong number of arguments. You need to pass 2 \n");
    return WRONG_ARGUMENTS_NUMBER;
  }

  if (MAX_FILE_NAME < strlen(argv[1])) {
    fprintf(stderr,
            "[Reversing] Wrong first argument. File name is too long! \n");
    return FILE_NAME_TOO_LONG;
  }

  if (MAX_FILE_NAME < strlen(argv[2])) {
    fprintf(stderr,
            "[Reversing] Wrong second argument. File name is too long! \n");
    return FILE_NAME_TOO_LONG;
  }

  if (access(argv[1], F_OK)) {
    fprintf(
        stderr,
        "[Reversing] Wrong first argument. You need to pass a file! \n %s \n",
        strerror(errno));
    return FILE_NOT_EXIST;
  }

  return NO_ERROR;
}

int reverse_string(char **string) {
  int length = strlen(*string);
  char *buffer = (char *)malloc(length);
  for (int i = 0; i < length; i++) {
    buffer[i] = *string[length - i - 1];
  }
  for (int i = 0; i < length; i++) {
    *string[i] = buffer[i];
  }
  free(buffer);
  return NO_ERROR;
}

int open_file(char *file_name, FILE **file) {
  *file = fopen(file_name, "rb");

  if (!*file) {
    fprintf(stderr, "[Reversing] Error while opening file! \n %s \n",
            strerror(errno));
    return FILE_NOT_OPEN;
  }

  return NO_ERROR;
}

int close_file(FILE *file) {
  if (fclose(file)) {
    fprintf(stderr, "[Reversing] Error while closing file! \n %s \n",
            strerror(errno));
    return FILE_NOT_CLOSED;
  }

  return NO_ERROR;
}

int create_file(char *file_name, FILE **file) {
  if (access(file_name, F_OK) == 0) {
    fprintf(stderr, "[Reversing] File already exists! \n %s \n",
            strerror(errno));
    return FILE_ALREADY_EXIST;
  }

  *file = fopen(file_name, "wb");

  if (!*file) {
    fprintf(stderr, "[Reversing] Error while creating file! \n %s \n",
            strerror(errno));
    return FILE_NOT_CREATED;
  }

  return NO_ERROR;
}

int reverse_file(char *source_file_name, char *destination_file_name) {
  FILE *source_file;
  FILE *destination_file;
  int ERROR_CODE = 0;
  long file_size = 0;

  if ((ERROR_CODE = open_file(source_file_name, &source_file))) {
    return ERROR_CODE;
  }

  if ((ERROR_CODE = create_file(destination_file_name, &destination_file))) {
    return ERROR_CODE;
  }
  _ = fseek(source_file, 0, SEEK_END);
  file_size = ftell(source_file);
  _ = fseek(source_file, 0, SEEK_SET);

  char *buffer = (char *)malloc(file_size);
  size_t end;
  size_t block_count = file_size / ((size_t)BLOCK_SIZE);
  size_t last_block_size = file_size % ((size_t)BLOCK_SIZE);
  for (size_t i = 0; i <= block_count; i++) {
    fseek(source_file, -BLOCK_SIZE * i, SEEK_END);
    end = fread(buffer, sizeof(char), BLOCK_SIZE, source_file);
    buffer[end] = 0;
    _ = reverse_string(&buffer);
    fprintf(destination_file, "%s", buffer);
  }

  fseek(source_file, 0, SEEK_SET);
  end = fread(buffer, sizeof(char), last_block_size, source_file);
  buffer[end] = 0;
  _ = reverse_string(&buffer);
  fprintf(destination_file, "%s", buffer);

  free(buffer);
  if ((ERROR_CODE = close_file(source_file))) {
    return ERROR_CODE;
  }
  if ((ERROR_CODE = close_file(destination_file))) {
    return ERROR_CODE;
  }
  return NO_ERROR;
}

int main(int argc, char *argv[]) {
  int result = check_arguments(argc, argv);
  if (result) {
    return result;
  }

  result = reverse_file(argv[1], argv[2]);
  if (result) {
    return result;
  }

  return 0;
}