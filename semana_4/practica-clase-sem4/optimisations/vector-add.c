#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

#define DEFAULT_ELEMS 1000000UL
#define DEFAULT_A_OFFSET 1.0
#define DEFAULT_B_OFFSET 2.0
#define DEFAULT_ITERATIONS 1000UL

volatile double benchmark_sink = 0.0;

typedef struct {
  double allocation;
  double fill_a;
  double fill_b;
  double add;
  double deallocation;
} Profile;

int allocate_vectors(double **a, double **b, double **result, size_t elems) {
  *a = malloc(elems * sizeof(**a));
  *b = malloc(elems * sizeof(**b));
  *result = malloc(elems * sizeof(**result));

  if (*a == NULL || *b == NULL || *result == NULL) {
    free(*a);
    free(*b);
    free(*result);
    *a = NULL;
    *b = NULL;
    *result = NULL;
    return 1;
  }

  return 0;
}

void fill_vector(double *vector, size_t elems, double offset) {
  for (size_t i = 0; i < elems; ++i) {
    vector[i] = (double)i + offset;
  }
}

void add_vectors(const double *a, const double *b, double *result, size_t elems) {
  for (size_t i = 0; i < elems; ++i) {
    result[i] = a[i] + b[i];
  }
}

void deallocate_vectors(double *a, double *b, double *result) {
  free(a);
  free(b);
  free(result);
}

int validate_result(const double *result, size_t elems, double a_offset,
                    double b_offset) {
  for (size_t i = 0; i < elems; ++i) {
    double expected = ((double)i + a_offset) + ((double)i + b_offset);

    if (result[i] != expected) {
      fprintf(stderr, "Validation error at index %zu: got %.6f expected %.6f\n",
              i, result[i], expected);
      return 1;
    }
  }

  return 0;
}

void print_profile(size_t elems, double a_offset, double b_offset,
                   size_t iterations, Profile profile) {
  double total = profile.allocation + profile.fill_a + profile.fill_b +
                 profile.add + profile.deallocation;

  printf("Vector addition with doubles\n");
  printf("Elements: %zu\n", elems);
  printf("Iterations for fill/add: %zu\n", iterations);
  printf("A offset: %.6f\n", a_offset);
  printf("B offset: %.6f\n", b_offset);
  printf("\nProfile:\n");
  printf("  allocation:   %.3f microseconds\n", profile.allocation * 1000000.0);
  printf("  fill A:       %.3f microseconds total, %.3f per iteration\n",
         profile.fill_a * 1000000.0,
         (profile.fill_a * 1000000.0) / (double)iterations);
  printf("  fill B:       %.3f microseconds total, %.3f per iteration\n",
         profile.fill_b * 1000000.0,
         (profile.fill_b * 1000000.0) / (double)iterations);
  printf("  add vectors:  %.3f microseconds total, %.3f per iteration\n",
         profile.add * 1000000.0,
         (profile.add * 1000000.0) / (double)iterations);
  printf("  deallocation: %.3f microseconds\n", profile.deallocation * 1000000.0);
  printf("  total:        %.3f microseconds\n", total * 1000000.0);
}

int main(int argc, char *argv[]) {
  size_t elems = DEFAULT_ELEMS;
  size_t iterations = DEFAULT_ITERATIONS;
  double a_offset = DEFAULT_A_OFFSET;
  double b_offset = DEFAULT_B_OFFSET;
  double *a = NULL;
  double *b = NULL;
  double *result = NULL;
  Profile profile = {0};

  if (argc > 1) {
    elems = strtoull(argv[1], NULL, 10);
  }
  if (argc > 2) {
    a_offset = strtod(argv[2], NULL);
  }
  if (argc > 3) {
    b_offset = strtod(argv[3], NULL);
  }
  if (argc > 4) {
    iterations = strtoull(argv[4], NULL, 10);
  }

  if (elems == 0) {
    fprintf(stderr, "Vector size must be greater than 0\n");
    return 1;
  }
  if (iterations == 0) {
    fprintf(stderr, "Iterations must be greater than 0\n");
    return 1;
  }

  double start = omp_get_wtime();
  if (allocate_vectors(&a, &b, &result, elems) != 0) {
    fprintf(stderr, "Could not allocate vectors for %zu elements\n", elems);
    return 1;
  }
  profile.allocation = omp_get_wtime() - start;

  start = omp_get_wtime();
  for (size_t i = 0; i < iterations; ++i) {
    fill_vector(a, elems, a_offset);
    benchmark_sink += a[elems - 1];
  }
  profile.fill_a = omp_get_wtime() - start;

  start = omp_get_wtime();
  for (size_t i = 0; i < iterations; ++i) {
    fill_vector(b, elems, b_offset);
    benchmark_sink += b[elems - 1];
  }
  profile.fill_b = omp_get_wtime() - start;

  start = omp_get_wtime();
  for (size_t i = 0; i < iterations; ++i) {
    add_vectors(a, b, result, elems);
    benchmark_sink += result[elems - 1];
  }
  profile.add = omp_get_wtime() - start;

  if (validate_result(result, elems, a_offset, b_offset) != 0) {
    deallocate_vectors(a, b, result);
    return 1;
  }

  start = omp_get_wtime();
  deallocate_vectors(a, b, result);
  profile.deallocation = omp_get_wtime() - start;

  print_profile(elems, a_offset, b_offset, iterations, profile);

  return 0;
}
