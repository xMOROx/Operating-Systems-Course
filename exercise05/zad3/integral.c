#include <fcntl.h>
#include <limits.h>
#include <linux/limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#define START 0
#define END 1
#define FIFO_PATH "/tmp/fifo"
#define BUFF_SIZE 255

int _;                    // dummy variable
char read_buf[BUFF_SIZE]; // buffer for reading from pipe

// ------ buffers for execv ------
char arg2[BUFF_SIZE]; // buffer for 1 param of execv
char arg3[BUFF_SIZE]; // buffer for 2 param of execv
// ------ buffers for execv ------

struct timespec timespec_diff(struct timespec start, struct timespec end) {
  struct timespec temp;
  if ((end.tv_nsec - start.tv_nsec) < 0) {
    temp.tv_sec = end.tv_sec - start.tv_sec - 1;
    temp.tv_nsec = 1000000000 + end.tv_nsec - start.tv_nsec;
  } else {
    temp.tv_sec = end.tv_sec - start.tv_sec;
    temp.tv_nsec = end.tv_nsec - start.tv_nsec;
  }
  return temp;
}

int main(int argc, char *argv[]) {

  if (argc != 3) {
    fprintf(stderr, "Usage: %s <width of range> n \n", argv[0]);
    return EXIT_FAILURE;
  }

  int n = atoi(argv[2]);
  double inv_n = 1.0 / n;
  double dx = strtod(argv[1], NULL);
  struct timespec timespec_buff_start, timespec_buff_end;

  clock_gettime(CLOCK_REALTIME, &timespec_buff_start);

  if (access(FIFO_PATH, F_OK) && mkfifo(FIFO_PATH, 0666)) {
    perror("mkfifo");
    return EXIT_FAILURE;
  } // create fifo if not exists

  for (int i = 0; i < n; i++) {

    if (!fork()) {
      // child process
      snprintf(arg2, BUFF_SIZE, "%lf", i * inv_n);       // a
      snprintf(arg3, BUFF_SIZE, "%lf", (i + 1) * inv_n); // b

      char *args[] = {
          "./prog_to_integrate.exe", arg2, arg3, argv[1], FIFO_PATH, NULL};
      int err = execv(args[0], args);

      if (err == -1) {
        perror("execl");
        return EXIT_FAILURE;
      }
    }
  }

  // parent process
  double sum = 0.0;

  int fd = open(FIFO_PATH, O_RDONLY);

  if (fd == -1) {
    perror("open");
    return EXIT_FAILURE;
  }

  int number_of_read_results = 0;

  size_t size;

  while (number_of_read_results < n) {
    char *token;
    char delim[] = ";";
    size = read(fd, read_buf, BUFF_SIZE);

    if (size == -1) {
      perror("read");
      return EXIT_FAILURE;
    }

    read_buf[size] = 0;

    token = strtok(read_buf, delim);

    while (token != NULL) {
      sum += strtod(token, NULL);
      token = strtok(NULL, delim);
      number_of_read_results++;
    }
  }

  close(fd);

  remove(FIFO_PATH);

  clock_gettime(CLOCK_REALTIME, &timespec_buff_end);
  struct timespec diff = timespec_diff(timespec_buff_start, timespec_buff_end);

  printf("Params: dx = %.15lf, n = %d \n", dx, n);

  printf("Result: %lf \n", sum);
  printf("Time: %lds %09ldns \n\n", diff.tv_sec, diff.tv_nsec);

  return EXIT_SUCCESS;
}
