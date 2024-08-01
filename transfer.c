#include "transfer.h"

#include <math.h>

static int _find_magnitude(struct vector* magnitude, double value, size_t* idx, double* frac)
{
    for(size_t i = 1; i < vector_size(magnitude); ++i)
    {
        double value1 = cabs(vector_get(magnitude, i - 1));
        double value2 = cabs(vector_get(magnitude, i));
        if((value1 - value) * (value2 - value) < 0)
        {
            *frac = -value1 / (value2 - value1);
            *idx = i;
            return 1;
        }
    }
    return 0;
}

static int _find_0dB(struct vector* magnitude, size_t* idx, double* frac)
{
    for(size_t i = 1; i < vector_size(magnitude); ++i)
    {
        double value1 = cabs(vector_get(magnitude, i - 1));
        double value2 = cabs(vector_get(magnitude, i));
        if(log10(value1) * log10(value2) < 0)
        {
            *frac = -log10(value1) / (log10(value2) - log10(value1));
            *idx = i;
            return 1;
        }
    }
    return 0;
}

static double complex _get_value(struct vector* values, size_t idx, double frac)
{
    return vector_get(values, idx) + frac * (vector_get(values, idx) - vector_get(values, idx - 1));
}

int transfer_phase_margin(struct vector* f, struct vector* H, double* result)
{
    size_t idx;
    double frac;
    if(_find_0dB(H, &idx, &frac))
    {
        struct vector* phase = vector_phase(H);
        *result = creal(_get_value(phase, idx, frac)) + 180;
        vector_destroy(phase);
        return 1;
    }
    return 0;
}

int transfer_unity_gain_frequency(struct vector* f, struct vector* H, double* result)
{
    size_t idx;
    double frac;
    if(_find_0dB(H, &idx, &frac))
    {
        *result = creal(_get_value(f, idx, frac));
        return 1;
    }
    return 0;
}

int transfer_lowpass_bandwidth(struct vector* f, struct vector* H, double* result)
{
    double lowfrequencygain = 20 * log10(cabs(vector_get(H, 0)));
    size_t idx;
    double frac;
    if(_find_magnitude(H, lowfrequencygain * 0.707945784384138, &idx, &frac))
    {
        *result = creal(_get_value(f, idx, frac));
        return 1;
    }
    return 0;
}
