#include <bits/time.h>
#include <limits.h>
#include <linux/limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#define START 0
#define END 1

int _;                    // dummy variable
char read_buf[PIPE_BUF];  // buffer for reading from pipe
char write_buf[PIPE_BUF]; // buffer for writing to pipe

typedef double (*func_t)(double x); // function pointer type

double to_integral(double x) { return 4 / (1 + x * x); }

double rectangular_integral(double dx, double a, double b, func_t f) {
  double sum = 0;
  for (double i = a; i < b; i += dx) {
    sum += f(i) * dx;
  }
  return sum;
}

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

  double width = strtod(argv[1], NULL);
  int n = atoi(argv[2]);
  double inv_n = 1.0 / n;
  struct timespec timespec_buff_start, timespec_buff_end;

  clock_gettime(CLOCK_REALTIME, &timespec_buff_start);

  int pipes[n]; // array of pipes

  for (int i = 0; i < n; i++) {
    int fd[2];

    if (pipe(fd) == -1) {
      perror("pipe");
      return EXIT_FAILURE;
    }

    if (fork()) { // child
      close(fd[END]);
      pipes[i] = fd[START];
    } else { // parent

      close(fd[START]);

      double result =
          rectangular_integral(width, i * inv_n, (i + 1) * inv_n, to_integral);

      size_t size = snprintf(write_buf, PIPE_BUF, "%lf", result);
      _ = write(fd[END], write_buf, size);

      exit(EXIT_SUCCESS);
    }
  }

  while (wait(NULL) > 0)
    ;

  double sum = 0.0;

  for (int i = 0; i < n; i++) {

    size_t size = read(pipes[i], read_buf, PIPE_BUF);
    read_buf[size] = 0;

    sum += strtod(read_buf, NULL);
  }

  clock_gettime(CLOCK_REALTIME, &timespec_buff_end);
  struct timespec diff = timespec_diff(timespec_buff_start, timespec_buff_end);

  printf("Params: width = %.15lf, n = %d \n", width, n);

  printf("Result: %lf \n", sum);
  printf("Time: %lds %09ldns \n\n", diff.tv_sec, diff.tv_nsec);

  return EXIT_SUCCESS;
}