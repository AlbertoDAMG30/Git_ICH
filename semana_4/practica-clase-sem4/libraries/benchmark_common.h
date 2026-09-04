#ifndef BENCHMARK_COMMON_H
#define BENCHMARK_COMMON_H

#include <stddef.h>

#define DEFAULT_ELEMS 1000000UL
#define DEFAULT_ITERATIONS 1000UL
#define DEFAULT_A_OFFSET 1.0
#define DEFAULT_B_OFFSET 2.0

typedef struct {
  double fill_a;
  double fill_b;
  double add;
  double total;
} Profile;

double elapsed_seconds(void);
int parse_args(int argc, char *argv[], size_t *elems, size_t *iterations,
               double *a_offset, double *b_offset);
double *allocate_vector(size_t elems);
void print_profile(const char *name, size_t elems, size_t iterations,
                   double a_offset, double b_offset, Profile profile);
int validate_result(const double *result, size_t elems, double a_offset,
                    double b_offset);

#endif
