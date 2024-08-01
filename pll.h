#ifndef PLL_PLL_H
#define PLL_PLL_H

#include <stddef.h>

struct pll_state;

// cost function
// receives (in this order):
// double phasemargin
// double bandwidth
// double Jrms
typedef double (*evaluator)(double, double, double);

struct pll_state* pll_create(void);
void pll_initialize(struct pll_state* state);
void pll_cleanup(struct pll_state* state);
void pll_set_input_output_frequencies(struct pll_state* state, double fref, double fsig);
void pll_set_eval_frequencies(struct pll_state* state, int flowerexp, int fupperexp, unsigned int pointsperdecade);
void pll_set_feedback_divider(struct pll_state* state, unsigned int factor);
void pll_set_reference_divider(struct pll_state* state, unsigned int factor);
void pll_add_parasitic_pole(struct pll_state* state, double pole);
void pll_set_phase_detector_gain(struct pll_state* state, double gain);
void pll_set_vco_gain(struct pll_state* state, double min_Kvco, double max_Kvco);
void pll_set_vco_noise(struct pll_state* state, double f0, double S0, double fc);
void pll_set_reference_noise(struct pll_state* state, double f0, double S0, double fc);
void pll_set_filter(struct pll_state* state, double Rs, double Cs, double Cx);
void pll_set_chargepump_gain(struct pll_state* state, double gm);
void pll_set_chargepump_noise(struct pll_state* state, double S0, double fc);
int pll_calculate(struct pll_state* state);
double pll_get_score(struct pll_state* state, evaluator eval);
void pll_print_result(struct pll_state* state);

#endif /* PLL_PLL_H */
