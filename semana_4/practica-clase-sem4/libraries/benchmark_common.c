#include "benchmark_common.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

double elapsed_seconds(void) {
  struct timespec now;
  clock_gettime(CLOCK_MONOTONIC, &now);
  return (double)now.tv_sec + (double)now.tv_nsec * 1e-9;
}

int parse_args(int argc, char *argv[], size_t *elems, size_t *iterations,
               double *a_offset, double *b_offset) {
  *elems = DEFAULT_ELEMS;
  *iterations = DEFAULT_ITERATIONS;
  *a_offset = DEFAULT_A_OFFSET;
  *b_offset = DEFAULT_B_OFFSET;

  if (argc > 1) {
    *elems = strtoull(argv[1], NULL, 10);
  }
  if (argc > 2) {
    *iterations = strtoull(argv[2], NULL, 10);
  }
  if (argc > 3) {
    *a_offset = strtod(argv[3], NULL);
  }
  if (argc > 4) {
    *b_offset = strtod(argv[4], NULL);
  }

  if (*elems == 0 || *iterations == 0) {
    fprintf(stderr, "Usage: ./program [elements] [iterations] [a_offset] [b_offset]\n");
    return 1;
  }

  return 0;
}

double *allocate_vector(size_t elems) {
  return malloc(elems * sizeof(double));
}

void print_profile(const char *name, size_t elems, size_t iterations,
                   double a_offset, double b_offset, Profile profile) {
  printf("%s\n", name);
  printf("Elements: %zu\n", elems);
  printf("Iterations: %zu\n", iterations);
  printf("A offset: %.6f\n", a_offset);
  printf("B offset: %.6f\n", b_offset);
  printf("\nProfile:\n");
  printf("  fill A: %.3f microseconds total, %.3f per iteration\n",
         profile.fill_a * 1000000.0,
         profile.fill_a * 1000000.0 / (double)iterations);
  printf("  fill B: %.3f microseconds total, %.3f per iteration\n",
         profile.fill_b * 1000000.0,
         profile.fill_b * 1000000.0 / (double)iterations);
  printf("  add:    %.3f microseconds total, %.3f per iteration\n",
         profile.add * 1000000.0,
         profile.add * 1000000.0 / (double)iterations);
  printf("  total:  %.3f microseconds\n", profile.total * 1000000.0);
}

int validate_result(const double *result, size_t elems, double a_offset,
                    double b_offset) {
  for (size_t i = 0; i < elems; ++i) {
    double expected = ((double)i + a_offset) + ((double)i + b_offset);

    if (result[i] != expected) {
      fprintf(stderr, "Error at index %zu: got %.6f expected %.6f\n", i,
              result[i], expected);
      return 1;
    }
  }

  return 0;
}
