#include <stdio.h>
#include <string.h>

#include "hall_queue.h"
#include "simulation_config.h"

const char *errors_queue_msg[] = {
    "[ERROR] Empty queue\n",
    "[ERROR] Full queue\n",
};

void error_queue_message(ERRORS_QUEUE error_code) {
  fprintf(stderr, "%s", errors_queue_msg[error_code]);
}

char queue_pop(char *queue) {
  if (queue_is_empty(queue)) {
    error_queue_message(EMPTY_QUEUE);
    return '\0';
  }
  char value = queue[0];
  memcpy(queue, queue + 1, strlen(queue) + 1);
  return value;
}
bool queue_push(char *queue, char value) {
  if (queue_is_full(queue)) {
    error_queue_message(FULL_QUEUE);
    return false;
  }
  size_t size = strlen(queue);

  queue[size] = value;
  queue[size + 1] = '\0';
  return true;
}
bool queue_is_empty(char *queue) { return strlen(queue) == 0; }
bool queue_is_full(char *queue) { return strlen(queue) + 1 == BUFFOR_SIZE; }