#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/msg.h>
#include <time.h>

#include "utils.h"

int current_available_client_id = 0;
int server_queue_id = -1;
key_t clients_queues[MAX_NO_OF_CLIENTS];

void server_initialize(MessageContener *msgContener);

void server_stop_client(int client_id);

void server_end_working();

void server_list_active_clients(int client_id);

void server_send_to_all(MessageContener *msgContener);

void server_send_to_one(MessageContener *msgContener);

void server_log_message(MessageContener *msgContener);

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

  key_t server_queue_key = ftok(HOME_PATH, SERVER_ID);

  if (server_queue_key == -1)
    handle_errors(FTOK_ERROR);

  server_queue_id = msgget(server_queue_key, IPC_CREAT | 0666);

  if (server_queue_id == -1)
    handle_errors(CREATE_QUEUE_ERROR);

  printf("Server queue id: %s%d%s \n", CYAN, server_queue_id, RESET);

  signal(SIGINT, server_end_working); // SIGINT - ctrl + c

  MessageContener *msgContener = malloc(sizeof(MessageContener));

  while (true) {
    if (msgrcv(server_queue_id, msgContener, MSG_SIZE, -6, 0) == -1)
      handle_errors(RECEIVE_ERROR);

    switch (msgContener->msgtype) {

    case INIT:
      server_initialize(msgContener);
      break;
    case STOP:
      server_stop_client(msgContener->client_id);
      server_log_message(msgContener);

      break;
    case LIST:
      server_list_active_clients(msgContener->client_id);
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

void server_initialize(MessageContener *msgContener) {
  while (clients_queues[current_available_client_id] != -1 &&
         current_available_client_id < MAX_NO_OF_CLIENTS - 1) {
    current_available_client_id++;
  }

  if (current_available_client_id == MAX_NO_OF_CLIENTS - 1 &&
      clients_queues[current_available_client_id] != -1) {
    msgContener->client_id = -1;
  } else {
    msgContener->client_id = current_available_client_id;
    clients_queues[current_available_client_id] = msgContener->client_queue_key;
    if (current_available_client_id < MAX_NO_OF_CLIENTS - 1) {
      current_available_client_id++;
    }
  }

  int client_queue_id = msgget(msgContener->client_queue_key, 0);
  if (client_queue_id == -1)
    handle_errors(OPEN_ERROR);

  if (msgsnd(client_queue_id, msgContener, MSG_SIZE, 0) == -1)
    handle_errors(SEND_TO_CLIENT_ERROR);

  server_log_message(msgContener);
}

void init_clients_queues() {
  for (int i = 0; i < MAX_NO_OF_CLIENTS; i++) {
    clients_queues[i] = -1;
  }
}

void server_stop_client(int client_id) {
  clients_queues[client_id] = -1;
  if (client_id < current_available_client_id) {
    current_available_client_id = client_id;
  }
}

void server_end_working() {
  MessageContener *msgContener = malloc(sizeof(MessageContener));

  for (int i = 0; i < MAX_NO_OF_CLIENTS; i++) {
    if (clients_queues[i] != -1) {
      msgContener->msgtype = STOP;
      int client_queue_id = msgget(clients_queues[i], 0);
      if (client_queue_id == -1)
        handle_errors(OPEN_ERROR);

      if (msgsnd(client_queue_id, msgContener, MSG_SIZE, 0) == -1)
        handle_errors(SEND_TO_CLIENT_ERROR);

      if (msgrcv(server_queue_id, msgContener, MSG_SIZE, STOP, 0) == -1)
        handle_errors(RECEIVE_ERROR);
    }
  }

  if (msgctl(server_queue_id, IPC_RMID, NULL) == -1)
    handle_errors(DELETE_ERROR);

  exit(EXIT_SUCCESS);
}

void server_list_active_clients(int client_id) {
  MessageContener *msgContener = malloc(sizeof(MessageContener));

  strcpy(msgContener->msg, "");

  for (int i = 0; i < MAX_NO_OF_CLIENTS; i++) {
    if (clients_queues[i] != -1) {
      sprintf(msgContener->msg + strlen(msgContener->msg),
              "ID %s%d%s is active... \n", BLUE, i, RESET);
    }
  }

  msgContener->msgtype = LIST;
  int client_queue_id = msgget(clients_queues[client_id], 0);
  if (client_queue_id == -1)
    handle_errors(OPEN_ERROR);

  if (msgsnd(client_queue_id, msgContener, MSG_SIZE, 0) == -1)
    handle_errors(SEND_TO_CLIENT_ERROR);
}

void server_send_to_all(MessageContener *msgContener) {
  for (int i = 0; i < MAX_NO_OF_CLIENTS; i++) {
    if (clients_queues[i] != -1 && msgContener->client_id != i) {
      int other_client_queue_id = msgget(clients_queues[i], 0);
      if (other_client_queue_id == -1)
        handle_errors(OPEN_ERROR);

      if (msgsnd(other_client_queue_id, msgContener, MSG_SIZE, 0) == -1)
        handle_errors(SEND_TO_CLIENT_ERROR);
    }
  }
}

void server_send_to_one(MessageContener *msgContener) {
  int other_client_queue_id =
      msgget(clients_queues[msgContener->other_client_id], 0);
  if (other_client_queue_id == -1)
    handle_errors(OPEN_ERROR);

  if (msgsnd(other_client_queue_id, msgContener, MSG_SIZE, 0) == -1)
    handle_errors(SEND_TO_CLIENT_ERROR);
}

void server_log_message(MessageContener *msgContener) {
  struct tm timeinfo = msgContener->time_of_msg_struct;

  FILE *logs = fopen("logs.txt", "a");

  if (logs == NULL)
    handle_errors(OPEN_FILE_ERROR);

  fprintf(logs, "Send at %d-%d-%d %02d:%02d:%02d: \n", timeinfo.tm_year + 1900,
          timeinfo.tm_mon + 1, timeinfo.tm_mday, timeinfo.tm_hour,
          timeinfo.tm_min, timeinfo.tm_sec);

  switch (msgContener->msgtype) {
  case INIT:
    if (msgContener->client_id == -1) {
      fprintf(logs,
              "Client %d tried to connect but server is "
              "full. \n",
              msgContener->client_id);
    } else {
      fprintf(logs, "Client of id:%d connected. \n", msgContener->client_id);
    }
  case STOP:
    fprintf(logs, "Client of id:%d disconnected. \n", msgContener->client_id);
    break;
  case LIST:
    fprintf(logs, "Client of id:%d requested list of active clients. \n",
            msgContener->client_id);
    break;
  case TO_ALL:
    fprintf(logs, "Client of id:%d sent message to all.\n Message: %s \n",
            msgContener->client_id, msgContener->msg);
    break;
  case TO_ONE:
    fprintf(
        logs,
        "Client of id:%d sent message to client of id:%d. \n Message: %s \n",
        msgContener->client_id, msgContener->other_client_id, msgContener->msg);
    break;
  default:
    fprintf(logs, "Unknown message type from cliend of id:%d. \n",
            msgContener->client_id);
  }

  if (fclose(logs) == EOF)
    handle_errors(CLOSE_FILE_ERROR);
}