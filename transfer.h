#ifndef PLL_TRANSFER_H
#define PLL_TRANSFER_H

#include "vector.h"

int transfer_find_0dB(struct vector* f, struct vector* H, double* result);
int transfer_unity_gain_frequency(struct vector* f, struct vector* H, double* result);
int transfer_phase_margin(struct vector* f, struct vector* H, double* result);
int transfer_lowpass_bandwidth(struct vector* f, struct vector* H, double* result);

#endif /* PLL_TRANSFER_H */
