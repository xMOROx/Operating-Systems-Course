#include "semaphore.h"
#include "simulation_config.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

const char *errors_sem_msg[] = {
    "[ERROR] while creating key with ftok()\n",
    "[ERROR] while creating semaphore with semget()\n",
    "[ERROR] while controlling semaphore with semctl()\n",
    "[ERROR] while acquiring with semop()\n",
    "[ERROR] while opening semaphore with sem_open()\n",
    "[ERROR] while closing semaphore with sem_close()\n",
    "[ERROR] while unlinking semaphore with sem_unlink()\n",
    "[ERROR] while waiting on semaphore with sem_wait()\n",
    "[ERROR] while posting on semaphore with sem_post()\n",
    "[ERROR] while releasing with semop()\n",
};

const char *perror_sem_msg[] = {
    "ftok",      "semget",     "semctl",   "semop",    "sem_open",
    "sem_close", "sem_unlink", "sem_wait", "sem_post", "semop"};

void error_handler_semaphore(ERRORS_SEMAPHORE error_code) {
  fprintf(stderr, "%s", errors_sem_msg[error_code]);
  perror(perror_sem_msg[error_code]);
  exit(error_code);
}

#ifdef POSIX

semaphore_t semaphore_create(const char *pathname, int init_val) {
  semaphore_t semaphore = sem_open(pathname, O_CREAT | O_EXCL, 0644, init_val);

  if (semaphore == SEM_FAILED) {
    error_handler_semaphore(SEMOPEN_ERROR);
  }

  return semaphore;
}
semaphore_t semaphore_open(const char *pathname) {
  semaphore_t semaphore = sem_open(pathname, 0);
  if (semaphore == SEM_FAILED) {
    error_handler_semaphore(SEMOPEN_ERROR);
  }

  return semaphore;
}

void semaphore_close(semaphore_t semaphore) {
  if (sem_close(semaphore) == -1) {
    error_handler_semaphore(SEMCLOSE_ERROR);
  }
}

void semaphore_remove(const char *pathname) { sem_unlink(pathname); }

void semaphore_post(semaphore_t semaphore) {
  if (sem_post(semaphore) == -1) {
    error_handler_semaphore(SEMPOST_ERROR);
  }
}

void semaphore_wait(semaphore_t semaphore) {
  if (sem_wait(semaphore) == -1) {
    error_handler_semaphore(SEMWAIT_ERROR);
  }
}
#endif

#ifdef SYSV

semaphore_t semaphore_create(const char *pathname, int init_val) {
  key_t key = ftok(IDENTIFICATOR, pathname[0]);
  if (key == -1) {
    error_handler_semaphore(FTOK_ERROR);
  }

  semaphore_t semaphore = semget(key, 1, IPC_CREAT | 0644);
  if (semaphore == -1) {
    error_handler_semaphore(SEMGET_ERROR);
  }

  if (semctl(semaphore, 0, SETVAL, init_val) == -1) {
    error_handler_semaphore(SEMCTL_ERROR);
  }

  return semaphore;
}

semaphore_t semaphore_open(const char *pathname) {
  key_t key = ftok(IDENTIFICATOR, pathname[0]);
  if (key == -1) {
    return EXIT_FAILURE;
  }

  return semget(key, 1, 0);
}
void semaphore_close(semaphore_t semaphore) {}
void semaphore_remove(const char *pathname) {
  semaphore_t semaphore = semaphore_open(pathname);
  semctl(semaphore, 0, IPC_RMID);
}
void semaphore_wait(semaphore_t semaphore) {
  struct sembuf sops = {.sem_num = 0, .sem_op = -1, .sem_flg = 0};

  if (semop(semaphore, &sops, 1) == -1) {
    error_handler_semaphore(SEMOP_WAIT_ERROR);
  }
}
void semaphore_post(semaphore_t semaphore) {
  struct sembuf sops = {.sem_num = 0, .sem_op = 1, .sem_flg = 0};
  if (semop(semaphore, &sops, 1) == -1) {
    error_handler_semaphore(SEMOP_POST_ERROR);
  }
}
#endif