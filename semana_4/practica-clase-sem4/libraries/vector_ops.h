#ifndef VECTOR_OPS_H
#define VECTOR_OPS_H

#include <stddef.h>

double value_from_index(size_t index, double offset);
double add_values(double a, double b);

void fill_vector(double *vector, size_t elems, double offset);
void add_vectors(const double *a, const double *b, double *result,
                 size_t elems);

#endif
