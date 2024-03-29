#include "change.h"
#include <bits/time.h>
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

int _; // to avoid unused variable warning

int check_arguments(int argc, char *argv[]) {
  if (argc != 5) {
    fprintf(stderr,
            "[Changing] Wrong number of arguments. You need to pass 5 \n");
    return WRONG_ARGUMENTS_NUMBER;
  }

  if (!isascii((int)(*argv[1]))) {
    fprintf(
        stderr,
        "[Changing] Wrong first argument. You need to pass a letter! \n %s \n",
        strerror(errno));
    return WRONG_FIRST_CHAR;
  }

  if (!isascii((int)(*argv[2]))) {
    fprintf(
        stderr,
        "[Changing] Wrong second argument. You need to pass a letter! \n %s \n",
        strerror(errno));
    return WRONG_SECOND_CHAR;
  }

  //   check if the 3 argument is a file

  if (access(argv[3], F_OK)) {
    fprintf(
        stderr,
        "[Changing] Wrong third argument. You need to pass a file! \n %s \n",
        strerror(errno));
    return SOURCE_FILE_NOT_FOUND;
  }

  return 0;
}

void change_char_in_buffer(char **buffer, const char old_char,
                           const char new_char) {
  char *buffer_ptr = *buffer;
  while (*buffer_ptr) {
    if (*buffer_ptr == old_char) {
      *buffer_ptr = new_char;
    }
    buffer_ptr++;
  }
}

int get_size_of_file(union Universal_File *file) {
#ifdef SYS
  struct stat file_stat;
  fstat(file->file_sys, &file_stat);
  return file_stat.st_size;
#else
  fseek(file->file_lib, 0, SEEK_END);
  int size = ftell(file->file_lib);
  fseek(file->file_lib, 0, SEEK_SET);
  return size;
#endif
}

int Universal_open_file(union Universal_File *file, char *file_name) {
#ifdef SYS
  file->file_sys = open(file_name, O_RDONLY);
  if (file->file_sys == -1) {
    fprintf(stderr, "[Changing] Error while opening source file: %s",
            strerror(errno));
    return SOURCE_FILE_NOT_FOUND;
  }
#else
  file->file_lib = fopen(file_name, "r");
  if (file->file_lib == NULL) {
    fprintf(stderr, "[Changing] Error while opening source file: %s",
            strerror(errno));
    return SOURCE_FILE_NOT_FOUND;
  }
#endif
  return 0;
}

int Universal_read_file(union Universal_File *file, char **buffer,
                        size_t size) {
#ifdef SYS
  return read(file->file_sys, *buffer, size);
#else
  return fread(*buffer, sizeof(char), size, file->file_lib);
#endif
}

int Universal_create_file(union Universal_File *file, char *file_name) {
  if (!access(file_name, F_OK)) {
    fprintf(stderr, "[Changing] Error destination file exists!: %s",
            strerror(errno));
    perror("Error destination file exists");
    return DESTINATION_FILE_NOT_CREATE;
  }

#ifdef SYS

  file->file_sys = creat(file_name, 0666);
  if (file->file_sys == -1) {
    fprintf(stderr, "[Changing] Error while creating destination file: %s",
            strerror(errno));
    return DESTINATION_FILE_NOT_CREATE;
  }
#else

  file->file_lib = fopen(file_name, "w");

#endif
  return 0;
}

int Universal_write_file(union Universal_File *file, char *buffer) {
#ifdef SYS
  _ = write(file->file_sys, buffer, BLOCK_SIZE);
#else
  _ = fwrite(buffer, sizeof(char), BLOCK_SIZE, file->file_lib);
#endif
  return 0;
}

int Universal_close_file(union Universal_File *file) {
#ifdef SYS
  _ = close(file->file_sys);
#else
  _ = fclose(file->file_lib);
#endif
  return 0;
}
int Universal_seek_file(union Universal_File *file, int offset) {
#ifdef SYS
  _ = lseek(file->file_sys, offset, SEEK_SET);
#else
  _ = fseek(file->file_lib, offset, SEEK_SET);
#endif
  return 0;
}

struct timespec timespec_diff(struct timespec start, struct timespec end) {
  struct timespec out;

  if ((end.tv_nsec - start.tv_nsec) < 0) {
    out.tv_sec = end.tv_sec - start.tv_sec - 1;
    out.tv_nsec = 1000000000 + end.tv_nsec - start.tv_nsec;
  } else {
    out.tv_sec = end.tv_sec - start.tv_sec;
    out.tv_nsec = end.tv_nsec - start.tv_nsec;
  }
  return out;
}

int change(struct Arguments *arguments) {
  char *buffer = (char *)calloc(BLOCK_SIZE, sizeof(char));

  char old_char = arguments->old_char;
  char new_char = arguments->new_char;
  char *source_file_name = arguments->source_file_name;
  char *destination_file_name = arguments->destination_file_name;
  union Universal_File *source_file =
      (union Universal_File *)calloc(1, sizeof(union Universal_File));
  union Universal_File *destination_file =
      (union Universal_File *)calloc(1, sizeof(union Universal_File));

  int _open_source_file = Universal_open_file(source_file, source_file_name);
  int _open_destination_file =
      Universal_create_file(destination_file, destination_file_name);

  if (_open_source_file != 0) {
    return _open_source_file;
  }

  if (_open_destination_file != 0) {
    return _open_destination_file;
  }

  int size = get_size_of_file(source_file);

  size_t count_blocks = size / ((size_t)BLOCK_SIZE);
  size_t end = 0;
  for (int i = 0; i < count_blocks; i++) {
    end = Universal_read_file(source_file, &buffer, BLOCK_SIZE);
    change_char_in_buffer(&buffer, old_char, new_char);
    buffer[end] = 0;
    _ = Universal_write_file(destination_file, buffer);
  }

  Universal_close_file(source_file);
  Universal_close_file(destination_file);

  free(buffer);
  free(source_file);
  free(destination_file);

  return 0;
}

int main(int argc, char *argv[]) {
  int check = check_arguments(argc, argv);
  if (check != 0) {
    return check;
  }
  struct timespec start, end;

  clock_gettime(CLOCK_REALTIME, &start);
  struct Arguments arguments = {
      .old_char = argv[1][0],
      .new_char = argv[2][0],
      .source_file_name = argv[3],
      .destination_file_name = argv[4],
  };

  int _change = change(&arguments);

  if (_change) {
    return _change;
  }
  clock_gettime(CLOCK_REALTIME, &end);
  // print time with s and ns
  struct timespec normalized = timespec_diff(start, end);
  printf("Time: %lds %ldns \n", normalized.tv_sec, normalized.tv_nsec);

  return 0;
}
