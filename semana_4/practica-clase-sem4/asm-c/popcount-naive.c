#include <omp.h>
#include <stdint.h>
#include <stdio.h>

#define ELEMS 10000

unsigned int popcount_c(uint64_t value) {
  unsigned int count = 0;

  for (int i = 0; i < 64; ++i) {
    count += value & 1;
    value >>= 1;
  }

  return count;
}

unsigned int popcount_asm(uint64_t value) {
  uint64_t result;

  asm("popcnt %1, %0" : "=r"(result) : "r"(value));

  return (unsigned int)result;
}

int main() {
  uint64_t values[ELEMS] = {0ul};
  uint16_t ones_naive[ELEMS] = {0};
  uint16_t ones_asm[ELEMS] = {0};

  for (int i = 0; i < ELEMS; ++i) {
    values[i] = i;
  }

  double start = omp_get_wtime();
  for (int i = 0; i < ELEMS; ++i) {
    ones_naive[i] = popcount_c(values[i]);
  }
  double end = omp_get_wtime();
  double naive_time = end - start;

  printf("Tiempo naive: %.6f microsegundos\n", naive_time * 1e6);

  start = omp_get_wtime();
  for (int i = 0; i < ELEMS; ++i) {
    ones_asm[i] = popcount_asm(values[i]);
  }
  end = omp_get_wtime();
  double asm_time = end - start;

  printf("Tiempo asm: %.6f microsegundos\n", asm_time * 1e6);

  for (int i = 0; i < ELEMS; ++i) {
    if (ones_naive[i] != ones_asm[i]) {
      printf("Error en indice %d: naive=%u asm=%u\n", i, ones_naive[i],
             ones_asm[i]);
      return 1;
    }
  }

  printf("Speedup: %.6f times\n", naive_time / asm_time);

  return 0;
}
