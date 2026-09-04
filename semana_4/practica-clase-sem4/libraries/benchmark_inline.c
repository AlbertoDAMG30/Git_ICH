#include "benchmark_common.h"
#include "vector_ops_inline.h"

#include <stdio.h>
#include <stdlib.h>

volatile double benchmark_sink = 0.0;

int main(int argc, char *argv[]) {
  size_t elems;
  size_t iterations;
  double a_offset;
  double b_offset;
  Profile profile = {0};

  if (parse_args(argc, argv, &elems, &iterations, &a_offset, &b_offset) != 0) {
    return 1;
  }

  double *a = allocate_vector(elems);
  double *b = allocate_vector(elems);
  double *result = allocate_vector(elems);

  if (a == NULL || b == NULL || result == NULL) {
    fprintf(stderr, "Could not allocate vectors\n");
    free(a);
    free(b);
    free(result);
    return 1;
  }

  double start_total = elapsed_seconds();

  double start = elapsed_seconds();
  for (size_t i = 0; i < iterations; ++i) {
    fill_vector_inline(a, elems, a_offset);
    benchmark_sink += a[elems - 1];
  }
  profile.fill_a = elapsed_seconds() - start;

  start = elapsed_seconds();
  for (size_t i = 0; i < iterations; ++i) {
    fill_vector_inline(b, elems, b_offset);
    benchmark_sink += b[elems - 1];
  }
  profile.fill_b = elapsed_seconds() - start;

  start = elapsed_seconds();
  for (size_t i = 0; i < iterations; ++i) {
    add_vectors_inline(a, b, result, elems);
    benchmark_sink += result[elems - 1];
  }
  profile.add = elapsed_seconds() - start;
  profile.total = elapsed_seconds() - start_total;

  if (validate_result(result, elems, a_offset, b_offset) != 0) {
    free(a);
    free(b);
    free(result);
    return 1;
  }

  print_profile("Inline version", elems, iterations, a_offset, b_offset,
                profile);

  free(a);
  free(b);
  free(result);

  return 0;
}
