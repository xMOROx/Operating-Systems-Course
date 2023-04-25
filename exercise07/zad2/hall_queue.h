#ifndef __HALL_QUEUE__
#define __HALL_QUEUE__

#include <stdbool.h>
#include <stdio.h>

typedef enum ERRORS_QUEUE {
  EMPTY_QUEUE = 0,
  FULL_QUEUE = 1,
} ERRORS_QUEUE;

char queue_pop(char *queue);
bool queue_push(char *queue, char value);
bool queue_is_empty(char *queue);
bool queue_is_full(char *queue);

#endif