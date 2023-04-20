#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/msg.h>
#include <time.h>

#include "utils.h"

#define NO_OF_COMMANDS 4
#define NO_OF_COMMANDS_WITH_ZERO_ARG 2
#define NO_OF_COMMANDS_WITH_ONE_ARG 1
#define NO_OF_COMMANDS_WITH_TWO_ARG 1

int client_queue_id;
char client_queue_name[CLIENT_QUEUE_NAME_SIZE];
mqd_t server_queue_descriptor;
mqd_t client_queue_decscriptor;

int handle_initialize();
void handle_stop();
void handle_list();
void handle_to_all(char *msg);
void handle_to_one(int other_client_id, char *msg);
void handle_server_message();
void parse_command(char *command);
void generate_client_queue_name();

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
            "%s[Error]%sWrong number of arguments. %sUsage%s: ./client.exe\n",
            RED, RESET, CYAN, RESET);
    exit(EXIT_FAILURE);
  }

  srand(time(NULL));
  generate_client_queue_name();

  client_queue_decscriptor = create_queue(client_queue_name);

  server_queue_descriptor = mq_open(SERVER_QUEUE_NAME, O_RDWR);

  if (server_queue_descriptor == -1)
    handle_errors(SERVER_NOT_RUNNING);

  client_queue_id = handle_initialize();

  signal(SIGINT, handle_stop);

  printf("%s[Info]%sClient started with name %s%s%s and id %s%d%s \n", BLUE,
         RESET, MAGENTA, client_queue_name, RESET, CYAN, client_queue_id,
         RESET);

  size_t len = 0;
  ssize_t read;

  char *command = NULL;

  while (true) {

    printf("%s>>>%s Enter command: ", GREEN, RESET);
    read = getline(&command, &len, stdin);
    command[read - 1] = '\0';

    handle_server_message();

    if (strcmp(command, "") == 0) {
      fprintf(stderr, "%s[Error]%sEmpty command. \n", RED, RESET);
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
          ;
        }
        command_handlers_one_arg[i - NO_OF_COMMANDS_WITH_ZERO_ARG](token);

        flag = 1;
        break;

      } else {
        token = strtok(NULL, " ");

        if (token == NULL) {
          break;
          ;
        }

        int other_client_id = atoi(token);
        token = strtok(NULL, "");

        if (token == NULL) {
          break;
          ;
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

void generate_client_queue_name() {
  client_queue_name[0] = '/';
  for (int i = 1; i < CLIENT_QUEUE_NAME_SIZE; i++) {
    client_queue_name[i] = random_char();
  }
}

int handle_initialize() {
  time_t time_value = time(NULL);
  MessageContener msgContener;

  msgContener.msgtype = INIT;
  msgContener.time_of_msg_struct = *localtime(&time_value);
  strcpy(msgContener.msg, client_queue_name);

  if (mq_send(server_queue_descriptor, (char *)&msgContener, MSG_SIZE, 0) == -1)
    handle_errors(SEND_TO_SERVER_ERROR);

  if (mq_receive(client_queue_decscriptor, (char *)&msgContener, MSG_SIZE,
                 NULL) == -1)
    handle_errors(RECEIVE_ERROR);

  int client_id = msgContener.client_id;

  if (client_id == -1)
    handle_errors(SERVER_FULL);

  return client_id;
}

void handle_stop() {
  time_t time_value = time(NULL);
  MessageContener msgContener;

  msgContener.msgtype = STOP;
  msgContener.time_of_msg_struct = *localtime(&time_value);
  msgContener.client_id = client_queue_id;

  if (mq_send(server_queue_descriptor, (char *)&msgContener, MSG_SIZE, 0) == -1)
    handle_errors(SEND_TO_SERVER_ERROR);

  if (mq_close(server_queue_descriptor) == -1)
    handle_errors(CLOSE_ERROR);

  exit(EXIT_SUCCESS);
}

void handle_list() {
  time_t time_value = time(NULL);
  MessageContener msgContener;

  msgContener.msgtype = LIST;
  msgContener.time_of_msg_struct = *localtime(&time_value);
  msgContener.client_id = client_queue_id;

  if (mq_send(server_queue_descriptor, (char *)&msgContener, MSG_SIZE, 0) == -1)
    handle_errors(SEND_TO_SERVER_ERROR);

  if (mq_receive(client_queue_decscriptor, (char *)&msgContener, MSG_SIZE,
                 NULL) == -1)
    handle_errors(RECEIVE_ERROR);
  printf("\n");
  printf("%s \n", msgContener.msg);
}

void handle_to_all(char *msg) {
  time_t time_value = time(NULL);
  MessageContener msgContener;

  msgContener.msgtype = TO_ALL;
  msgContener.time_of_msg_struct = *localtime(&time_value);
  msgContener.client_id = client_queue_id;

  strcpy(msgContener.msg, msg);

  if (mq_send(server_queue_descriptor, (char *)&msgContener, MSG_SIZE, 0) == -1)
    handle_errors(SEND_TO_SERVER_ERROR);
}

void handle_to_one(int other_client_id, char *msg) {
  time_t time_value = time(NULL);
  MessageContener msgContener;

  msgContener.msgtype = TO_ONE;
  msgContener.time_of_msg_struct = *localtime(&time_value);

  msgContener.client_id = client_queue_id;
  msgContener.other_client_id = other_client_id;

  strcpy(msgContener.msg, msg);

  if (mq_send(server_queue_descriptor, (char *)&msgContener, MSG_SIZE, 0) == -1)
    handle_errors(SEND_TO_SERVER_ERROR);
}

void handle_server_message() {
  MessageContener msgContener;
  struct timespec time_of_message;
  clock_gettime(CLOCK_REALTIME, &time_of_message);

  while (mq_timedreceive(client_queue_decscriptor, (char *)&msgContener,
                         MSG_SIZE, NULL, &time_of_message) != -1) {
    if (msgContener.msgtype == STOP) {
      printf("%s[Info]%sServer stopped. Exiting... \n", BLUE, RESET);
      handle_stop();
    } else {
      printf("%s[Receive]%sMessage from: %s%d%s has been sent at "
             "%s%02d:%02d:%02d%s \n",
             GREEN, RESET, CYAN, msgContener.client_id, RESET, BLUE,
             msgContener.time_of_msg_struct.tm_hour,
             msgContener.time_of_msg_struct.tm_min,
             msgContener.time_of_msg_struct.tm_sec, RESET);
      printf("Message: %s \n", msgContener.msg);
    }
  }
}
