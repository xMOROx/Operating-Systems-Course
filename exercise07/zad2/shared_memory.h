#ifndef __SHARED_MEMORY__
#define __SHARED_MEMORY__
#include <stdbool.h>

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

char *shared_memory_create(const char *pathname, int size);
bool shared_memory_detach(char *shared_memory);
bool shared_memory_destroy(const char *pathname);

typedef enum ERRORS_SHARED_MEMORY {
  FTOK_SHM_ERROR = 0,
  SHMAT_ERROR = 1,
  SHMDT_ERROR = 2,
  SHMCTL_ERROR = 3,
  SHM_OPEN_ERROR = 4,
  FTRUNCATE_ERROR = 5,
  MMAP_ERROR = 6,
  MUNMAP_ERROR = 7,
  SHM_UNLINK_ERROR = 8,
  GET_FILE_DESCRIPTOR_ERROR = 9,
  SHMGET_ERROR = 10,
} ERRORS_SHARED_MEMORY;

#endif