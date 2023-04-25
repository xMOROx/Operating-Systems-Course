#ifndef __SEMAPHORE__
#define __SEMAPHORE__
#include <fcntl.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <unistd.h>
#ifdef POSIX
typedef sem_t *semaphore_t;
#endif
#ifdef SYSV
typedef int semaphore_t;
#endif

semaphore_t semaphore_create(const char *pathname, int init_val);
semaphore_t semaphore_open(const char *pathname);
void semaphore_close(semaphore_t semaphore);
void semaphore_remove(const char *pathname);
void semaphore_wait(semaphore_t semaphore);
void semaphore_post(semaphore_t semaphore);

typedef enum ERRORS_SEMAPHORE {
  FTOK_ERROR = 0,
  SEMGET_ERROR = 1,
  SEMCTL_ERROR = 2,
  SEMOP_WAIT_ERROR = 3,
  SEMOPEN_ERROR = 4,
  SEMCLOSE_ERROR = 5,
  SEMUNLINK_ERROR = 6,
  SEMWAIT_ERROR = 7,
  SEMPOST_ERROR = 8,
  SEMOP_POST_ERROR = 9,
} ERRORS_SEMAPHORE;

#endif
