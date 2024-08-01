#ifndef PLL_VECTOR_H
#define PLL_VECTOR_H

#include <complex.h>
#include <stddef.h>

struct vector;

struct vector* vector_create(size_t size, double complex value);
struct vector* vector_destroy(struct vector* vector);
void vector_set(struct vector* vector, size_t idx, double complex value);
void vector_set_all(struct vector* vector, double complex value);
void vector_copy_values(struct vector* vector, const struct vector* other);
double complex vector_get(struct vector* vector, size_t idx);
size_t vector_size(const struct vector* vector);
struct vector* vector_copy(const struct vector* vector);
void vector_add_scalar(struct vector* vector, double complex value);
void vector_scale(struct vector* vector, double complex factor);
void vector_add(struct vector* a, const struct vector* b);
void vector_multiply(struct vector* multiplicand, const struct vector* multiplier);
void vector_divide(struct vector* dividend, const struct vector* divisor);
void vector_abs(struct vector* vector);
void vector_abs_squared(struct vector* vector);
struct vector* vector_magnitude(const struct vector* vector);
struct vector* vector_phase(const struct vector* vector);
void vector_print(const struct vector* vector);
struct vector* vector_logspace(double a, double b, unsigned int N);

#endif /* PLL_VECTOR_H */
