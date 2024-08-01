#include <assert.h>
#include <complex.h>
#include <math.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>

#include "constants.h"

struct vector {
    double complex* values;
    size_t size;
};

struct vector* _create(size_t size)
{
    struct vector* vector = malloc(sizeof(*vector));
    vector->values = malloc(size * sizeof(*vector->values));
    vector->size = size;
    return vector;
}

struct vector* vector_create(size_t size, double complex value)
{
    struct vector* vector = _create(size);
    for(size_t i = 0; i < vector->size; ++i)
    {
        vector->values[i] = value;
    }
    return vector;
}

void vector_destroy(struct vector* vector)
{
    free(vector->values);
    free(vector);
}

void vector_set(struct vector* vector, size_t idx, double complex value)
{
    vector->values[idx] = value;
}

void vector_set_all(struct vector* vector, double complex value)
{
    for(size_t i = 0; i < vector->size; ++i)
    {
        vector->values[i] = value;
    }
}

void vector_copy_values(struct vector* vector, const struct vector* other)
{
    assert(vector->size == other->size);
    for(size_t i = 0; i < vector->size; ++i)
    {
        vector->values[i] = other->values[i];
    }
}

double complex vector_get(struct vector* vector, size_t idx)
{
    return vector->values[idx];
}

size_t vector_size(const struct vector* vector)
{
    return vector->size;
}

struct vector* vector_copy(const struct vector* vector)
{
    struct vector* new = malloc(sizeof(*new));
    new->values = malloc(vector->size * sizeof(*new->values));
    new->size = vector->size;
    for(size_t i = 0; i < vector->size; ++i)
    {
        new->values[i] = vector->values[i];
    }
    return new;
}

void vector_add_scalar(struct vector* vector, double complex value)
{
    for(size_t i = 0; i < vector->size; ++i)
    {
        vector->values[i] += value;
    }
}

void vector_scale(struct vector* vector, double complex factor)
{
    for(size_t i = 0; i < vector->size; ++i)
    {
        vector->values[i] *= factor;
    }
}

void vector_add(struct vector* a, const struct vector* b)
{
    assert(a->size == b->size);
    for(size_t i = 0; i < a->size; ++i)
    {
        a->values[i] += b->values[i];
    }
}

void vector_multiply(struct vector* multiplicand, const struct vector* multiplier)
{
    assert(multiplicand->size == multiplier->size);
    for(size_t i = 0; i < multiplicand->size; ++i)
    {
        multiplicand->values[i] *= multiplier->values[i];
    }
}

void vector_divide(struct vector* dividend, const struct vector* divisor)
{
    assert(dividend->size == divisor->size);
    for(size_t i = 0; i < dividend->size; ++i)
    {
        dividend->values[i] /= divisor->values[i];
    }
}

void vector_abs(struct vector* vector)
{
    for(size_t i = 0; i < vector->size; ++i)
    {
        vector->values[i] = cabs(vector->values[i]);
    }
}

void vector_abs_squared(struct vector* vector)
{
    for(size_t i = 0; i < vector->size; ++i)
    {
        double value = cabs(vector->values[i]);
        vector->values[i] = value * value;
    }
}

struct vector* vector_magnitude(const struct vector* vector)
{
    struct vector* result = vector_copy(vector);
    for(size_t i = 0; i < result->size; ++i)
    {
        result->values[i] = cabs(result->values[i]);
    }
    return result;
}

struct vector* vector_phase(struct vector* vector)
{
    struct vector* result = vector_copy(vector);
    for(size_t i = 0; i < result->size; ++i)
    {
        result->values[i] = 180 / CONSTANTS_PI * carg(result->values[i]);
    }
    for(size_t i = 1; i < result->size; ++i)
    {
        if(creal(result->values[i]) - creal(result->values[i - 1]) > 180)
        {
            for(size_t j = i; j < result->size; ++j)
            {
                result->values[j] = result->values[j] - 360;
            }
        }
        else if(creal(result->values[i]) - creal(result->values[i - 1]) < -180)
        {
            for(size_t j = i; j < result->size; ++j)
            {
                result->values[j] = result->values[j] + 360;
            }
        }
    }
    return result;
}

void vector_print(const struct vector* vector)
{
    for(size_t i = 0; i < vector->size; ++i)
    {
        double complex value = vector->values[i];
        printf("%g + %g * i\n", creal(value), cimag(value));
    }
}

struct vector* vector_logspace(double a, double b, unsigned int N)
{
    struct vector* x = _create(N);
    double factor = pow(10, (b - a) / (N - 1));
    for(size_t i = 0; i < N; ++i)
    {
        x->values[i] = pow(10, a) * pow(factor, i);
    }
    return x;
}

