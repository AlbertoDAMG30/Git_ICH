#ifndef VECTOR_OPS_INLINE_H
#define VECTOR_OPS_INLINE_H

#include <stddef.h>

static inline double value_from_index_inline(size_t index, double offset) {
  return (double)index + offset;
}

static inline double add_values_inline(double a, double b) {
  return a + b;
}

static inline void fill_vector_inline(double *vector, size_t elems,
                                      double offset) {
  for (size_t i = 0; i < elems; ++i) {
    vector[i] = value_from_index_inline(i, offset);
  }
}

static inline void add_vectors_inline(const double *a, const double *b,
                                      double *result, size_t elems) {
  for (size_t i = 0; i < elems; ++i) {
    result[i] = add_values_inline(a[i], b[i]);
  }
}

#endif
