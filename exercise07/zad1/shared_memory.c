#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <unistd.h>

#include "shared_memory.h"

const char *errors_shm_msg[] = {
    "[ERROR] while creating key with ftok()\n",
    "[ERROR] while attaching shared memory with shmat()\n",
    "[ERROR] while detaching shared memory with shmdt()\n",
    "[ERROR] while controlling shared memory with shmctl()\n",
    "[ERROR] while opening shared memory with shm_open()\n",
    "[ERROR] while truncating shared memory with ftruncate()\n",
    "[ERROR] while mapping shared memory with mmap()\n",
    "[ERROR] while unmapping shared memory with munmap()\n",
    "[ERROR] while unlinking shared memory with shm_unlink()\n",
    "[ERROR] while getting file descriptor with shm_open()\n",
    "[ERROR] while getting file descriptor\n",
    "[ERROR] while getting shared memory with shmget()\n",
};

const char *perror_shm_msg[] = {"ftok",       "shmat",     "shmdt", "shmctl",
                                "shm_open",   "ftruncate", "mmap",  "munmap",
                                "shm_unlink", "",          "shmget"};

void error_handler_shared_memory(ERRORS_SHARED_MEMORY error_code) {
  fprintf(stderr, "%s", errors_shm_msg[error_code]);
  perror(perror_shm_msg[error_code]);
  exit(error_code);
}

void error_shm_message(ERRORS_SHARED_MEMORY error_code) {
  fprintf(stderr, "%s", errors_shm_msg[error_code]);
}

#ifdef SYSV
static int shared_memory_get(const char *pathname, int size) {
  key_t key = ftok(pathname, 0);
  if (key == -1) {
    return -1;
  }

  return shmget(key, size, IPC_CREAT | 0666);
}

char *shared_memory_create(const char *pathname, int size) {
  int shmid = shared_memory_get(pathname, size);
  if (shmid == -1) {
    error_shm_message(GET_FILE_DESCRIPTOR_ERROR);
    return NULL;
  }

  char *shared_memory = shmat(shmid, NULL, 0);
  if (shared_memory == (char *)-1) {
    error_shm_message(SHMAT_ERROR);
    return NULL;
  }

  return shared_memory;
}

bool shared_memory_detach(char *shared_memory) {
  return shmdt(shared_memory) != -1;
}
bool shared_memory_destroy(const char *pathname) {
  int shmid = shared_memory_get(pathname, 0);
  if (shmid == -1) {
    error_shm_message(GET_FILE_DESCRIPTOR_ERROR);
    return false;
  }

  return shmctl(shmid, IPC_RMID, NULL) != -1;
}
#endif

#ifdef POSIX
static int shared_memory_get(const char *pathname, int size) {
  int fd = shm_open(pathname, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
  if (fd == -1) {
    error_shm_message(SHM_OPEN_ERROR);
    return -1;
  }
  if (ftruncate(fd, size) == -1) {
    error_shm_message(FTRUNCATE_ERROR);
    return -1;
  }

  return fd;
}

char *shared_memory_create(const char *pathname, int size) {
  int shmid = shared_memory_get(pathname, size);
  if (shmid == -1) {
    return NULL;
  }

  char *shared_memory =
      mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, shmid, 0);

  return shared_memory;
}
bool shared_memory_detach(char *shared_memory) {
  return munmap(shared_memory, sizeof(shared_memory)) != -1;
}
bool shared_memory_destroy(const char *pathname) {
  return shm_unlink(pathname) != -1;
}
#endif