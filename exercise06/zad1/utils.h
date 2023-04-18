#ifndef __UTILS__
#define __UTILS__

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/msg.h>
#include <time.h>

#define SERVER_ID 1
#define MAX_NO_OF_CLIENTS 5
#define MAX_MSG_SIZE 512
#define HOME_PATH getenv("HOME")
#define THE_OLDEST_MESSAGE 0
#define MAX_CLIENT_QUEUE_KEY 128
#define true 1

typedef struct MessageContener {
  long msgtype;
  key_t client_queue_key;
  int client_id;
  int other_client_id;
  char msg[MAX_MSG_SIZE];
  struct tm time_of_msg;

} MessageContener;

typedef enum MessageType {
  INIT = 1,
  LIST = 2,
  TO_ALL = 3,
  TO_ONE = 4,
  STOP = 5
} MessageType;

const long MSG_SIZE = sizeof(MessageContener);

#endif
