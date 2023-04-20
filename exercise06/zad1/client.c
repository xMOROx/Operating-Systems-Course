#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/msg.h>
#include <time.h>
#include <unistd.h>

#include "utils.h"

#define NO_OF_COMMANDS 4
#define NO_OF_COMMANDS_WITH_ZERO_ARG 2
#define NO_OF_COMMANDS_WITH_ONE_ARG 1
#define NO_OF_COMMANDS_WITH_TWO_ARG 1

key_t client_queue_key;
int client_queue_id;
int server_queue_id;
int client_id;

int handle_initialize();
void handle_stop();
void handle_list();
void handle_to_all(char *msg);
void handle_to_one(int other_client_id, char *msg);
void handle_server_message();
void parse_command(char *command);

const char *commands[4] = {"LIST", "STOP", "2ALL", "2ONE"};

void (*command_handlers_zero_arg[2])() = {
    handle_list,
    handle_stop,
};
void (*command_handlers_one_arg[1])(char *) = {
    handle_to_all,
};

void (*command_handlers_two_arg[1])(int, char *) = {
    handle_to_one,
};

int main(int argc, char **argv) {
  if (argc != 1) {
    fprintf(stderr,
            "%s[ERROR]%s Wrong number of arguments. %sUsage%s: ./client.exe\n",
            RED, RESET, CYAN, RESET);
    exit(EXIT_FAILURE);
  }

  srand(time(NULL));

  client_queue_key = ftok(HOME_PATH, rand() % MAX_CLIENT_QUEUE_KEY + 1);

  if (client_queue_key == -1)
    handle_errors(FTOK_ERROR);

  client_queue_id = msgget(client_queue_key, IPC_CREAT | 0666);

  if (client_queue_id == -1)
    handle_errors(CREATE_QUEUE_ERROR);

  key_t server_queue_key = ftok(HOME_PATH, SERVER_ID);

  if (server_queue_key == -1)
    handle_errors(FTOK_ERROR);

  server_queue_id = msgget(server_queue_key, 0);

  if (server_queue_id == -1)
    handle_errors(SERVER_NOT_RUNNING);

  client_id = handle_initialize();

  printf("Client id: %s%d%s \n", CYAN, client_id, RESET);

  signal(SIGINT, handle_stop);

  size_t len = 0;
  ssize_t read;

  char *command = NULL;

  while (true) {

    printf("%s>>> %sEnter command: ", GREEN, RESET);
    read = getline(&command, &len, stdin);
    command[read - 1] = '\0';

    handle_server_message();

    if (strcmp(command, "") == 0) {
      fprintf(stderr, "%s[ERROR]%sEmpty command. \n", RED, RESET);
      continue;
    }
    parse_command(command);
  }
}

void parse_command(char *command) {
  char *token = strtok(command, " ");
  int flag = 0;
  for (int i = 0; i < NO_OF_COMMANDS; i++) {
    if (strcmp(token, commands[i]) == 0) {
      if (i < NO_OF_COMMANDS_WITH_ZERO_ARG) {
        command_handlers_zero_arg[i]();

        flag = 1;
        break;

      } else if (i <
                 NO_OF_COMMANDS_WITH_ZERO_ARG + NO_OF_COMMANDS_WITH_ONE_ARG) {
        token = strtok(NULL, "");

        if (token == NULL) {
          break;
        }
        command_handlers_one_arg[i - NO_OF_COMMANDS_WITH_ZERO_ARG](token);

        flag = 1;
        break;

      } else {
        token = strtok(NULL, " ");

        if (token == NULL) {
          break;
        }

        int other_client_id = atoi(token);
        token = strtok(NULL, "");

        if (token == NULL) {
          break;
        }

        command_handlers_two_arg[i - NO_OF_COMMANDS_WITH_ZERO_ARG -
                                 NO_OF_COMMANDS_WITH_ONE_ARG](other_client_id,
                                                              token);

        flag = 1;
        break;
      }
    }
  }
  if (flag == 0)
    fprintf(stderr, "%s[ERROR]%sWrong command. \n", RED, RESET);
}

int handle_initialize() {
  time_t time_value = time(NULL);
  MessageContener *msgContener = malloc(sizeof(MessageContener));

  msgContener->msgtype = INIT;
  msgContener->time_of_msg_struct = *localtime(&time_value);
  msgContener->client_queue_key = client_queue_key;

  if (msgsnd(server_queue_id, msgContener, MSG_SIZE, 0) == -1)
    handle_errors(SEND_TO_SERVER_ERROR);

  if (msgrcv(client_queue_id, msgContener, MSG_SIZE, THE_OLDEST_MESSAGE, 0) ==
      -1)
    handle_errors(RECEIVE_ERROR);

  int client_id = msgContener->client_id;

  if (client_id == -1)
    handle_errors(SERVER_FULL);

  return client_id;
}

void handle_stop() {
  time_t time_value = time(NULL);
  MessageContener *msgContener = malloc(sizeof(MessageContener));

  msgContener->msgtype = STOP;
  msgContener->time_of_msg_struct = *localtime(&time_value);
  msgContener->client_id = client_id;

  if (msgsnd(server_queue_id, msgContener, MSG_SIZE, 0) == -1)
    handle_errors(SEND_TO_SERVER_ERROR);

  if (msgctl(client_queue_id, IPC_RMID, NULL) == -1)
    handle_errors(DELETE_ERROR);

  exit(EXIT_SUCCESS);
}

void handle_list() {
  time_t time_value = time(NULL);
  MessageContener *msgContener = malloc(sizeof(MessageContener));

  msgContener->msgtype = LIST;
  msgContener->time_of_msg_struct = *localtime(&time_value);
  msgContener->client_id = client_id;

  if (msgsnd(server_queue_id, msgContener, MSG_SIZE, 0) == -1)
    handle_errors(SEND_TO_SERVER_ERROR);

  if (msgrcv(client_queue_id, msgContener, MSG_SIZE, THE_OLDEST_MESSAGE, 0) ==
      -1)
    handle_errors(RECEIVE_ERROR);
  printf("\n");
  printf("%s", msgContener->msg);
}

void handle_to_all(char *msg) {
  time_t time_value = time(NULL);
  MessageContener *msgContener = malloc(sizeof(MessageContener));

  msgContener->msgtype = TO_ALL;
  msgContener->time_of_msg_struct = *localtime(&time_value);
  msgContener->client_id = client_id;

  strcpy(msgContener->msg, msg);

  if (msgsnd(server_queue_id, msgContener, MSG_SIZE, 0) == -1)
    handle_errors(SEND_TO_SERVER_ERROR);
}

void handle_to_one(int other_client_id, char *msg) {
  time_t time_value = time(NULL);
  MessageContener *msgContener = malloc(sizeof(MessageContener));

  msgContener->msgtype = TO_ONE;
  msgContener->time_of_msg_struct = *localtime(&time_value);

  msgContener->client_id = client_id;
  msgContener->other_client_id = other_client_id;

  strcpy(msgContener->msg, msg);

  if (msgsnd(server_queue_id, msgContener, MSG_SIZE, 0) == -1)
    handle_errors(SEND_TO_SERVER_ERROR);
}

void handle_server_message() {
  MessageContener *msgContener = malloc(sizeof(MessageContener));

  while (msgrcv(client_queue_id, msgContener, MSG_SIZE, THE_OLDEST_MESSAGE,
                IPC_NOWAIT) >= 0) {
    if (msgContener->msgtype == STOP) {
      printf("%s[Info]%sServer send stop message. Exiting...\n", BLUE, RESET);
      handle_stop();
    } else {
      struct tm *time_of_msg = &msgContener->time_of_msg_struct;
      printf("%s[Receive]%sMessage from: %s%d%s has been sent at  "
             "%s%02d:%02d:%02d%s.\n",
             GREEN, RESET, CYAN, msgContener->client_id, RESET, BLUE,
             time_of_msg->tm_hour, time_of_msg->tm_min, time_of_msg->tm_sec,
             RESET);
      printf("Message content: %s \n", msgContener->msg);
    }
  }
}
