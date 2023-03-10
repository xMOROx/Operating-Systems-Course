#include "../zad1/wcCount.h"
#include <bits/time.h>
#include <ctype.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/times.h>
#include <time.h>
#include <unistd.h>

#define MAX_INPUT_LENGHT 2048
#define INITIAL_COMMAND "init"
#define COUNT_COMMAND "count"
#define SHOW_COMMAND "show"
#define DELETE_COMMAND "delete"
#define DESTROY_COMMAND "destroy"
#define EXIT_COMMAND "exit"

// colors
// #define RED "\x1b[31m"
// #define GREEN "\x1b[32m"
// #define DEFAULT "\x1b[0m"
// #define BLUE "\x1b[34m"

// w celach raportu zmieniam kolory na puste stringi
#define RED ""
#define GREEN ""
#define DEFAULT ""
#define BLUE ""

_Bool initialized = 0;
_Bool errorOccured = 0;

volatile sig_atomic_t flag = 0;

void handler(int signum) { flag = 1; }

int strToInt(const char *str) {
  int num = 0;
  int sign = 1;
  int decimal = 0;
  const char *p = str;
  if (*p == '-') {
    sign = -1;
    ++p;
  }

  while (*p != '\0') {
    if (isdigit(*p)) {
      if (decimal) {
        // decimal point already found
        return -1;
      }
      num = num * 10 + (*p - '0');
    } else if (*p == '.') {
      if (decimal) {
        // more than one decimal point
        return -1;
      }
      decimal = 1;
    } else {
      // non-digit character
      return -1;
    }
    ++p;
  }
  return num * sign;
}

void print_timespec(struct timespec start, struct timespec end,
                    struct tms tmsStart, struct tms tmsEnd) {
  double realTime =
      (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
  double userTime =
      (tmsEnd.tms_utime - tmsStart.tms_utime) / (double)sysconf(_SC_CLK_TCK);
  double sysTime =
      (tmsEnd.tms_stime - tmsStart.tms_stime) / (double)sysconf(_SC_CLK_TCK);
  printf("Real time: %.5lfs \nUser time %.5lfs \nSystem time %.5lfs \n",
         realTime, userTime, sysTime);
}

void inputParser(char *input, char **command, char **argument,
                 blocks **blocks) {
  int size, index;
  *command = strtok(input, " ");

  if (!*command) {
    printf("%sREPL ERROR >>> No command given \n", RED);
    errorOccured = 1;
    return;
  }

  *argument = strtok(NULL, " ");

  if (!*argument && (strcmp(*command, INITIAL_COMMAND) == 0 ||
                     strcmp(*command, COUNT_COMMAND) == 0 ||
                     strcmp(*command, SHOW_COMMAND) == 0 ||
                     strcmp(*command, DELETE_COMMAND) == 0)) {
    printf("%sREPL ERROR >>> No argument given \n", RED);
    errorOccured = 1;
    return;
  }

  if (strcmp(*command, INITIAL_COMMAND) == 0) {
    if (initialized) {
      printf("%sREPL ERROR >>> Already initialized \n", RED);
      errorOccured = 1;
      return;
    }

    size = strToInt(*argument);

    if (size <= 0) {
      printf("%sREPL ERROR >>> Invalid size \n", RED);
      errorOccured = 1;
      return;
    }

    printf("%sREPL OUTPUT >>> Initializing structure with size %d \n", GREEN,
           size);

    *blocks = createBlock(size);
    initialized = 1;
    return;
  }
  if (!initialized && (strcmp(*command, COUNT_COMMAND) == 0 ||
                       strcmp(*command, SHOW_COMMAND) == 0 ||
                       strcmp(*command, DELETE_COMMAND) == 0 ||
                       strcmp(*command, DESTROY_COMMAND) == 0)) {
    printf("%sREPL ERROR >>> Structure not initialized \n", RED);
    errorOccured = 1;
    return;
  }

  if (strcmp(*command, COUNT_COMMAND) == 0) {
    switch (countWords(*argument, *blocks)) {
    case FAILURE:
      printf("%sREPL ERROR >>> Failure \n", RED);
      errorOccured = 1;
      return;
    case FILE_NOT_FOUND:
      printf("%sREPL ERROR >>> File not found \n", RED);
      errorOccured = 1;
      return;
    case FILE_READ_ERROR:
      printf("%sREPL ERROR >>> File read error \n", RED);
      errorOccured = 1;
      return;
    case FILE_REMOVE_ERROR:
      printf("%sREPL ERROR >>> File remove error \n", RED);
      errorOccured = 1;
      return;
    case ALLOCATION_MEMORY_ERROR:
      printf("%sREPL ERROR >>> Allocation memory error \n", RED);
      errorOccured = 1;
      return;
    case BLOCK_ADD_ERROR:
      printf("%sREPL ERROR >>> Block add error \n", RED);
      errorOccured = 1;
      return;
    }
    return;

  } else if (strcmp(*command, SHOW_COMMAND) == 0) {
    index = strToInt(*argument);

    if (index < 0 || index >= (*blocks)->maxBlocks) {
      printf("%sREPL ERROR >>> Invalid index \n", RED);
      errorOccured = 1;
      return;
    }

    void *block = getBlockContent(*blocks, index);

    if (!block) {
      printf("%sREPL ERROR >>> Invalid index. Block doesn't exists! \n", RED);
      errorOccured = 1;
      return;
    }

    printf("REPL OUTPUT >>> %s \n", (char *)block);

    return;

  } else if (strcmp(*command, DELETE_COMMAND) == 0) {

    if (!argument || strcmp(*argument, "index") != 0) {
      printf("%sREPL ERROR >>> Invalid command \n", RED);
      errorOccured = 1;
      return;
    }
    *argument = strtok(NULL, " ");
    if (!*argument) {
      printf("%sREPL ERROR >>> Invalid command. Index is needed! \n", RED);
      errorOccured = 1;
      return;
    }

    index = strToInt(*argument);

    if (index < 0 || index >= (*blocks)->maxBlocks) {
      printf("%sREPL ERROR >>> Invalid index \n", RED);
      errorOccured = 1;
      return;
    }

    if (removeBlock(*blocks, index) == FAILURE) {
      printf("%sREPL ERROR >>> Failure during removing process! \n", RED);
      errorOccured = 1;
      return;
    }
    return;

  } else if (strcmp(*command, DESTROY_COMMAND) == 0) {

    freeBlockArray(*blocks);
    initialized = 0;

    return;
  } else if (strcmp(*command, EXIT_COMMAND) == 0) {

    freeBlockArray(*blocks);
    flag = 1;

    return;
  }

  printf("%sREPL ERROR >>> Unknown command \n", RED);
  printf("%sREPL HELPER >>> Available commands: \n", BLUE);
  printf("REPL HELPER >>> init [size], count, show, delete index, "
         "destroy, exit \n");
  printf("%s\t1. init [size] - initialize library \n", GREEN);
  printf("\t2. count [file] - count words in file \n");
  printf("\t3. show [index] - show content from index \n");
  printf("\t4. delete index [index] - delete content from index and "
         "shift it to keep continuity of structure\n");
  printf("\t5. destroy - destroy array of content \n");
  errorOccured = 1;
  return;
}

int main() {
  blocks *blocks = NULL;

  char input[MAX_INPUT_LENGHT];
  char *command = NULL;
  char *argument = NULL;

  struct timespec start, end;
  struct tms tmsStart, tmsEnd;

  signal(SIGINT, handler); // exit on ctrl + c

  while (1) {
    errorOccured = 0;
    printf("%sREPL>>> ", GREEN);
    fgets(input, MAX_INPUT_LENGHT, stdin);

    input[strlen(input) - 1] = '\0'; // remove newline

    times(&tmsStart);
    clock_gettime(CLOCK_REALTIME, &start);

    inputParser(input, &command, &argument, &blocks);

    if (flag)
      break;

    if (errorOccured)
      continue;

    clock_gettime(CLOCK_REALTIME, &end);

    times(&tmsEnd);
    print_timespec(start, end, tmsStart, tmsEnd);
  }
}
