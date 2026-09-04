#include "vector_ops.h"

double value_from_index(size_t index, double offset) {
  return (double)index + offset;
}

double add_values(double a, double b) {
  return a + b;
}

void fill_vector(double *vector, size_t elems, double offset) {
  for (size_t i = 0; i < elems; ++i) {
    vector[i] = value_from_index(i, offset);
  }
}

void add_vectors(const double *a, const double *b, double *result,
                 size_t elems) {
  for (size_t i = 0; i < elems; ++i) {
    result[i] = add_values(a[i], b[i]);
  }
}
