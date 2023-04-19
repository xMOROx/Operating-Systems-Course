#ifndef __UTILS__
#define __UTILS__

#include <fcntl.h>
#include <mqueue.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>

#define SERVER_QUEUE_NAME "/server_queue"
#define MAX_NO_OF_CLIENTS 5
#define MAX_MSG_SIZE 512
#define true 1
#define CLIENT_QUEUE_NAME_SIZE 10

typedef struct MessageContener {
  int client_id;
  int other_client_id;
  long msgtype;
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
  SERVER_FULL = 9
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
                           "Server is full.\nTry again later.\n"};

const char *perror_type[] = {"mq_send", "mq_send",   "mq_receive", "mq_close",
                             "mq_open", "mq_unlink", "fopen",      "fclose",
                             "",        ""};

const u_long MSG_SIZE = sizeof(MessageContener);

char random_char() { return (char)(rand() % 26 + 65); }

void handle_errors(int error_code) {
  fprintf(stderr, "%s", error_msg[error_code]);
  perror(perror_type[error_code]);
  exit(EXIT_FAILURE);
}

mqd_t create_queue(const char *queue_name) {
  struct mq_attr attr;
  attr.mq_maxmsg = MAX_NO_OF_CLIENTS;
  attr.mq_msgsize = MSG_SIZE;
  attr.mq_curmsgs = 0;
  attr.mq_flags = 0;

  mqd_t queue = mq_open(queue_name, O_RDWR | O_CREAT | O_EXCL, 0666, &attr);
  if (queue == -1) {
    handle_errors(OPEN_ERROR);
  }
  return queue;
}

#endif
