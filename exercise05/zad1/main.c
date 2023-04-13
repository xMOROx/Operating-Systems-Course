#include <bits/time.h>
#include <limits.h>
#include <linux/limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#define BUFF_SIZE 1024

bool is_valid_email(char *email) {
  char *at = strchr(email, '@');
  char *dot = strrchr(email, '.');

  return (at != NULL && dot != NULL && dot > at);
}

int parse_one_argument(char **argv) {
  char *command = "mail -H";

  if (strcmp(argv[1], "date") == 0) {
    command = "mail -H | sort -k 3";
  } else if (strcmp(argv[1], "sender") == 0) {
    command = "mail -H | sort -k 2";
  } else {
    fprintf(
        stderr,
        "Invalid argument: %s is not a valid argument for 1 argument usage\n",
        argv[1]);
    fprintf(stderr, "Usage %s --help for more info \n", argv[1]);
    return EXIT_FAILURE;
  }

  FILE *mail = popen(command, "w");

  if (mail == NULL) {
    fprintf(stderr, "Error: Could not open mail\n");
    return EXIT_FAILURE;
  }

  char line[BUFF_SIZE];
  while (fgets(line, BUFF_SIZE, mail) != NULL) {
    printf("%s", line);
  }
  pclose(mail);

  return EXIT_SUCCESS;
}

int parse_three_arguments(int argc, char **argv) {
  FILE *mail = popen("mail", "w");

  if (mail == NULL) {
    fprintf(stderr, "Error: Could not open mail\n");
    return EXIT_FAILURE;
  }

  char *to = argv[1];
  char *subject = argv[2];
  char buffor[BUFF_SIZE];

  if (!is_valid_email(to)) {
    fprintf(stderr, "Error: Invalid email address\n");
    pclose(mail);
    return EXIT_FAILURE;
  }

  for (int i = 3; i < argc; i++) {
    strcat(buffor, argv[i]);
    strcat(buffor, " ");
  }

  char command[strlen(to) + strlen(subject) + strlen(buffor) + 100];

  snprintf(command,
           sizeof(char) * (strlen(to) + strlen(subject) + strlen(buffor) + 100),
           "echo '%s' | mail -s '%s' '%s'", buffor, subject, to);
  printf("%s \n", command);
  system(command);
  // free(command);
  pclose(mail);

  return EXIT_SUCCESS;
}

int main(int argc, char **argv) {

  if (strcmp(argv[1], "--help") == 0) {
    fprintf(stderr, "Usage of 1 argument: %s <date>/<sender>\n", argv[0]);
    fprintf(stderr, "Usage of 3 arguments: %s <to> <title> <body>\n", argv[0]);
    return EXIT_FAILURE;
  }

  if (argc == 1) {
    fprintf(stderr, "Usage: %s <date>/<sender> OR %s <to> <title> <body>\n",
            argv[0], argv[0]);
    return EXIT_FAILURE;
  }

  if (argc == 2)
    parse_one_argument(argv);
  else if (argc > 3)
    parse_three_arguments(argc, argv);
  else {
    fprintf(stderr, "Usage of 1 argument: %s <date>/<sender>\n", argv[0]);
    fprintf(stderr, "Usage of 3 or more arguments: %s <to> <title> <body>\n",
            argv[0]);
  }
  return EXIT_SUCCESS;
}
