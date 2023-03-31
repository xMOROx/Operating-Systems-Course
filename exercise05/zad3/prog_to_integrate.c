#include <fcntl.h>
#include <limits.h>
#include <linux/limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#define BUFF_SIZE 255

char write_buf[BUFF_SIZE]; // buffer for writing to pipe
int _;

typedef double (*func_t)(double x); // function pointer type

double to_integral(double x) { return 4 / (1 + x * x); }

double rectangular_integral(double dx, double a, double b, func_t f) {
  double sum = 0;
  for (double i = a; i < b; i += dx) {
    sum += f(i) * dx;
  }

  return sum;
}

int main(int argc, char **argv) {

  if (argc != 5) {
    fprintf(stderr, "Usage: %s <a> <b> <dx>  \n", argv[0]);
    exit(EXIT_FAILURE);
  }

  double a = strtod(argv[1], NULL);
  double b = strtod(argv[2], NULL);
  double dx = strtod(argv[3], NULL);
  char *fifo_path = argv[4];
  char delim[] = ";";

  double result = rectangular_integral(dx, a, b, to_integral);
  size_t size = snprintf(write_buf, BUFF_SIZE, "%lf%s", result, delim);

  int fd = open(fifo_path, O_WRONLY);

  if (fd == -1) {
    perror("open");
    return EXIT_FAILURE;
  }

  _ = write(fd, write_buf, size);

  close(fd);

  return EXIT_SUCCESS;
}
