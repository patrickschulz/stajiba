#ifndef PLL_NOISE
#define PLL_NOISE

#include "vector.h"

struct vector* noise_PSD_constant(struct vector* S, struct vector* f, double S0);
struct vector* noise_PSD_white_flicker(struct vector* S, struct vector* f, double Sfloor, double fc);
struct vector* noise_PSD_10dB_per_decade(struct vector* S, struct vector* f, double df0, double S0);
struct vector* noise_PSD_20dB_per_decade(struct vector* S, struct vector* f, double df0, double S0);
struct vector* noise_PSD_30dB_per_decade(struct vector* S, struct vector* f, double df0, double S0);
double noise_L_to_S(double L);
double noise_trapzS(struct vector* f, struct vector* S);
double noise_RMSjitterS(double f0, struct vector* f, struct vector* S);

#endif /* PLL_NOISE */
