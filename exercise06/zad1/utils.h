#ifndef __UTILS__
#define __UTILS__

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/msg.h>
#include <sys/types.h>
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
  struct tm time_of_msg_struct;

} MessageContener;

typedef enum MessageType {
  INIT = 1,
  LIST = 2,
  TO_ALL = 3,
  TO_ONE = 4,
  STOP = 5
} MessageType;

typedef enum error_code {
  SEND_TO_SERVER_ERROR = 0,
  SEND_TO_CLIENT_ERROR = 1,
  RECEIVE_ERROR = 2,
  CLOSE_ERROR = 3,
  OPEN_ERROR = 4,
  DELETE_ERROR = 5,
  OPEN_FILE_ERROR = 6,
  CLOSE_FILE_ERROR = 7,
  SERVER_NOT_RUNNING = 8,
  SERVER_FULL = 9,
  FTOK_ERROR = 10,
  CREATE_QUEUE_ERROR = 11,
} error_code;

const char *error_msg[] = {"Error while sending message to server.\n",
                           "Error while sending message to client.\n",
                           "Error while receiving message.\n",
                           "Error while closing queue.\n",
                           "Error while opening queue.\n",
                           "Error while deleting queue.\n",
                           "Error while opening file.\n",
                           "Error while closing file.\n",
                           "Server is not running: ",
                           "Server is full.\nTry again later.\n",
                           "Error while creating queue key.\n",
                           "Error while creating queue.\n"};

const char *perror_type[] = {"msgsnd", "msgsnd", "msgrcv", "msgctl", "msgget",
                             "msgctl", "fopen",  "fclose", "",       "",
                             "ftok",   "msgget"

};

void handle_errors(int error_code) {
  fprintf(stderr, "%s", error_msg[error_code]);
  perror(perror_type[error_code]);
  exit(EXIT_FAILURE);
}

const u_long MSG_SIZE = sizeof(MessageContener);

#endif
