#include <errno.h>
#include <mqueue.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "utils.h"

int current_available_client_id = 0;
char *clients_queues[MAX_NO_OF_CLIENTS];
mqd_t server_queue;

void server_initialize(MessageContener msgContener);

void server_stop_client(int client_id);

void server_end_working();

void server_list_active_clients(int client_id);

void server_send_to_all(MessageContener msgContener);

void server_send_to_one(MessageContener msgContener);

void server_log_message(MessageContener msgContener);

void init_clients_queues();

int main(int argc, char *argv[]) {
  if (argc != 1) {
    fprintf(stderr,
            "%s[ERROR]%s Wrong number of arguments. %sUsage%s: ./server.exe\n",
            RED, RESET, CYAN, RESET);
    exit(EXIT_FAILURE);
  }

  init_clients_queues();

  printf("%s[Info]%sServer starting... \n", BLUE, RESET);

  mq_unlink(SERVER_QUEUE_NAME);

  server_queue = create_queue(SERVER_QUEUE_NAME);

  signal(SIGINT, server_end_working); // SIGINT - ctrl + c

  MessageContener msgContener;

  while (true) {
    if (mq_receive(server_queue, (char *)&msgContener, MSG_SIZE, NULL) == -1)
      handle_errors(RECEIVE_ERROR);

    switch (msgContener.msgtype) {

    case INIT:
      server_initialize(msgContener);
      break;
    case STOP:
      server_stop_client(msgContener.client_id);
      server_log_message(msgContener);

      break;
    case LIST:
      server_list_active_clients(msgContener.client_id);
      server_log_message(msgContener);

      break;
    case TO_ALL:
      server_send_to_all(msgContener);
      server_log_message(msgContener);

      break;
    case TO_ONE:
      server_send_to_one(msgContener);
      server_log_message(msgContener);

      break;
    default:
      printf("%s[Error]%sUnknown message type. \n", RED, RESET);
      break;
    }
  }

  exit(EXIT_SUCCESS);
}

void server_initialize(MessageContener msgContener) {
  while (clients_queues[current_available_client_id] != NULL &&
         current_available_client_id < MAX_NO_OF_CLIENTS - 1) {

    current_available_client_id++;
  }

  if (current_available_client_id == MAX_NO_OF_CLIENTS - 1 &&
      clients_queues[current_available_client_id] != NULL) {

    msgContener.client_id = -1;

  } else {

    msgContener.client_id = current_available_client_id;
    clients_queues[current_available_client_id] =
        (char *)calloc(CLIENT_QUEUE_NAME_SIZE, sizeof(char));
    strcpy(clients_queues[current_available_client_id], msgContener.msg);

    if (current_available_client_id < MAX_NO_OF_CLIENTS - 1) {
      current_available_client_id++;
    }
  }

  mqd_t client_queue_descriptor = mq_open(msgContener.msg, O_RDWR);

  if (client_queue_descriptor == -1)
    handle_errors(OPEN_ERROR);

  if (mq_send(client_queue_descriptor, (char *)&msgContener, MSG_SIZE, 0) == -1)
    handle_errors(SEND_TO_CLIENT_ERROR);

  if (mq_close(client_queue_descriptor) == -1)
    handle_errors(CLOSE_ERROR);

  server_log_message(msgContener);
}

void init_clients_queues() {
  for (int i = 0; i < MAX_NO_OF_CLIENTS; i++) {
    clients_queues[i] = NULL;
  }
}

void server_stop_client(int client_id) {
  clients_queues[client_id] = NULL;
  if (client_id < current_available_client_id) {
    current_available_client_id = client_id;
  }
}

void server_end_working() {
  MessageContener msgContener;

  for (int i = 0; i < MAX_NO_OF_CLIENTS; i++) {
    if (clients_queues[i] != NULL) {
      msgContener.msgtype = STOP;

      mqd_t client_queue_descriptor = mq_open(clients_queues[i], O_RDWR);

      if (client_queue_descriptor == -1)
        handle_errors(OPEN_ERROR);

      if (mq_send(client_queue_descriptor, (char *)&msgContener, MSG_SIZE, 0) ==
          -1)
        handle_errors(SEND_TO_CLIENT_ERROR);

      if (mq_receive(server_queue, (char *)&msgContener, MSG_SIZE, NULL) == -1)
        handle_errors(RECEIVE_ERROR);

      if (mq_close(client_queue_descriptor) == -1)
        handle_errors(CLOSE_ERROR);
    }
  }

  if (mq_close(server_queue) == -1)
    handle_errors(CLOSE_ERROR);

  if (mq_unlink(SERVER_QUEUE_NAME) == -1)
    handle_errors(UNLINK_ERROR);

  exit(EXIT_SUCCESS);
}

void server_list_active_clients(int client_id) {
  MessageContener msgContener;
  strcpy(msgContener.msg, "");

  for (int i = 0; i < MAX_NO_OF_CLIENTS; i++) {
    if (clients_queues[i] != NULL) {
      sprintf(msgContener.msg + strlen(msgContener.msg),
              "ID %s%d%s is active... \n", BLUE, i, RESET);
    }
  }
  mqd_t client_queue_descriptor = mq_open(clients_queues[client_id], O_RDWR);

  if (client_queue_descriptor == -1)
    handle_errors(OPEN_ERROR);

  if (mq_send(client_queue_descriptor, (char *)&msgContener, MSG_SIZE, 0) == -1)
    handle_errors(SEND_TO_CLIENT_ERROR);

  if (mq_close(client_queue_descriptor) == -1)
    handle_errors(CLOSE_ERROR);
}

void server_send_to_all(MessageContener msgContener) {
  for (int i = 0; i < MAX_NO_OF_CLIENTS; i++) {
    if (clients_queues[i] != NULL && msgContener.client_id != i) {
      mqd_t client_queue_descriptor = mq_open(clients_queues[i], O_RDWR);

      if (client_queue_descriptor == -1)
        handle_errors(OPEN_ERROR);

      if (mq_send(client_queue_descriptor, (char *)&msgContener, MSG_SIZE, 0) ==
          -1)
        handle_errors(SEND_TO_CLIENT_ERROR);

      if (mq_close(client_queue_descriptor) == -1)
        handle_errors(CLOSE_ERROR);
    }
  }
}

void server_send_to_one(MessageContener msgContener) {
  mqd_t other_client_queue_descriptor =
      mq_open(clients_queues[msgContener.other_client_id], O_RDWR);

  if (other_client_queue_descriptor == -1)
    handle_errors(OPEN_ERROR);

  if (mq_send(other_client_queue_descriptor, (char *)&msgContener, MSG_SIZE,
              0) == -1)
    handle_errors(SEND_TO_CLIENT_ERROR);

  if (mq_close(other_client_queue_descriptor) == -1)
    handle_errors(CLOSE_ERROR);
}

void server_log_message(MessageContener msgContener) {
  struct tm timeinfo = msgContener.time_of_msg_struct;

  FILE *logs = fopen("logs.txt", "a");

  if (logs == NULL)
    handle_errors(OPEN_ERROR);

  fprintf(logs, "Send at %d-%d-%d %02d:%02d:%02d: \n", timeinfo.tm_year + 1900,
          timeinfo.tm_mon + 1, timeinfo.tm_mday, timeinfo.tm_hour,
          timeinfo.tm_min, timeinfo.tm_sec);

  switch (msgContener.msgtype) {
  case INIT:
    if (msgContener.client_id == -1) {
      fprintf(logs,
              "Client %d tried to connect but server is "
              "full. \n",
              msgContener.client_id);
    } else {
      fprintf(logs, "Client of id:%d connected. \n", msgContener.client_id);
    }
  case STOP:
    fprintf(logs, "Client of id:%d disconnected. \n", msgContener.client_id);
    break;
  case LIST:
    fprintf(logs, "Client of id:%d requested list of active clients. \n",
            msgContener.client_id);
    break;
  case TO_ALL:
    fprintf(logs, "Client of id:%d sent message to all.\n Message: %s \n",
            msgContener.client_id, msgContener.msg);
    break;
  case TO_ONE:
    fprintf(
        logs,
        "Client of id:%d sent message to client of id:%d. \n Message: %s \n",
        msgContener.client_id, msgContener.other_client_id, msgContener.msg);
    break;
  default:
    fprintf(logs, "Unknown message type from cliend of id:%d. \n",
            msgContener.client_id);
  }

  if (fclose(logs) == -1)
    handle_errors(CLOSE_ERROR);
}