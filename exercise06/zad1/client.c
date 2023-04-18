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
    fprintf(stderr, "Wrong number of arguments. Usage: ./client.exe\n");
    exit(EXIT_FAILURE);
  }

  srand(time(NULL));

  client_queue_key = ftok(HOME_PATH, rand() % MAX_CLIENT_QUEUE_KEY + 1);
  client_queue_id = msgget(client_queue_key, IPC_CREAT | 0666);

  key_t server_queue_key = ftok(HOME_PATH, SERVER_ID);
  server_queue_id = msgget(server_queue_key, 0);

  client_id = handle_initialize();

  printf("Client id: %d \n", client_id);

  signal(SIGINT, handle_stop);

  size_t len = 0;
  ssize_t read;

  char *command = NULL;

  while (true) {

    printf("Enter command: ");
    read = getline(&command, &len, stdin);
    command[read - 1] = '\0';

    handle_server_message();

    if (strcmp(command, "") == 0) {
      fprintf(stderr, "Empty command. \n");
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
          fprintf(stderr, "Wrong command. \n");
          continue;
        }
        command_handlers_one_arg[i - NO_OF_COMMANDS_WITH_ZERO_ARG](token);

        flag = 1;
        break;

      } else {
        token = strtok(NULL, " ");

        if (token == NULL) {
          fprintf(stderr, "Wrong command. \n");
          continue;
        }

        int other_client_id = atoi(token);
        token = strtok(NULL, "");

        if (token == NULL) {
          fprintf(stderr, "Wrong command. \n");
          continue;
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
    fprintf(stderr, "Wrong command. \n");
}

int handle_initialize() {
  time_t time_value = time(NULL);
  MessageContener *msgContener = malloc(sizeof(MessageContener));

  msgContener->msgtype = INIT;
  msgContener->time_of_msg = *localtime(&time_value);
  msgContener->client_queue_key = client_queue_key;
  msgsnd(server_queue_id, msgContener, MSG_SIZE, 0);
  msgrcv(client_queue_id, msgContener, MSG_SIZE, THE_OLDEST_MESSAGE, 0);

  int client_id = msgContener->client_id;

  if (client_id == -1) {
    fprintf(stderr, "Server is full. Exiting... \n");
    exit(EXIT_FAILURE);
  }

  return client_id;
}

void handle_stop() {
  time_t time_value = time(NULL);
  MessageContener *msgContener = malloc(sizeof(MessageContener));

  msgContener->msgtype = STOP;
  msgContener->time_of_msg = *localtime(&time_value);
  msgContener->client_id = client_id;

  msgsnd(server_queue_id, msgContener, MSG_SIZE, 0);
  msgctl(client_queue_id, IPC_RMID, NULL);

  exit(EXIT_SUCCESS);
}

void handle_list() {
  time_t time_value = time(NULL);
  MessageContener *msgContener = malloc(sizeof(MessageContener));

  msgContener->msgtype = LIST;
  msgContener->time_of_msg = *localtime(&time_value);
  msgContener->client_id = client_id;

  msgsnd(server_queue_id, msgContener, MSG_SIZE, 0);
  msgrcv(client_queue_id, msgContener, MSG_SIZE, THE_OLDEST_MESSAGE, 0);

  printf("%s", msgContener->msg);
}

void handle_to_all(char *msg) {
  time_t time_value = time(NULL);
  MessageContener *msgContener = malloc(sizeof(MessageContener));

  msgContener->msgtype = TO_ALL;
  msgContener->time_of_msg = *localtime(&time_value);
  msgContener->client_id = client_id;

  strcpy(msgContener->msg, msg);

  msgsnd(server_queue_id, msgContener, MSG_SIZE, 0);
}

void handle_to_one(int other_client_id, char *msg) {
  time_t time_value = time(NULL);
  MessageContener *msgContener = malloc(sizeof(MessageContener));

  msgContener->msgtype = TO_ONE;
  msgContener->time_of_msg = *localtime(&time_value);

  msgContener->client_id = client_id;
  msgContener->other_client_id = other_client_id;

  strcpy(msgContener->msg, msg);

  msgsnd(server_queue_id, msgContener, MSG_SIZE, 0);
}

void handle_server_message() {
  MessageContener *msgContener = malloc(sizeof(MessageContener));

  while (msgrcv(client_queue_id, msgContener, MSG_SIZE, THE_OLDEST_MESSAGE,
                IPC_NOWAIT) >= 0) {
    if (msgContener->msgtype == STOP) {
      printf("Server send stop message. Exiting...\n");
      handle_stop();
    } else {
      struct tm *time_of_msg = &msgContener->time_of_msg;
      printf("Message from: %d has been sent at  %02d:%02d:%02d.\n ",
             msgContener->client_id, time_of_msg->tm_hour, time_of_msg->tm_min,
             time_of_msg->tm_sec);
      printf("Message content: %s \n", msgContener->msg);
    }
  }
}
