#include "pll.h"

#include <stdio.h>
#include <stdlib.h>

#include "constants.h"
#include "engineering.h"
#include "noise.h"
#include "transfer.h"
#include "vector.h"

struct pll_state {
    int flowerexp;
    int fupperexp;
    unsigned int pointsperdecade;
    double fref;
    double fsig;

    unsigned int N;
    unsigned int M;

    struct vector* f;
    struct vector* s;

    // Phase Detector
    double detectorgain;
    double Sphasedetector0;
    struct vector* Hdetector;
    struct vector* Nphasedetector;
    struct vector* Sphasedetector;
    struct vector* Stot_phasedetector;

    // Charge Pump
    double gm;
    double Scp0;
    double fccp;
    struct vector* Hcp;
    struct vector* Ncp;
    struct vector* Scp;
    struct vector* Stot_cp;

    // oscillator
    double min_Kvco;
    double max_Kvco;
    double Svco0;
    double dfvco0;
    double dfvco0fc;
    struct vector* Hvco;
    struct vector* Nvco;
    struct vector* Svco;
    struct vector* Stot_vco;

    // Filter
    double Rf;
    double Cf;
    double Cfx;
    struct vector* Nfilter;
    struct vector* Sfilter;
    struct vector* Stot_filter;

    // Reference
    double Sref0;
    double dfref0;
    double dfref0fc;
    struct vector* Href;
    struct vector* Nref;
    struct vector* Sref;
    struct vector* Stot_ref;

    // Parasitic Poles
    double* parpoles;
    size_t numparpoles;
    struct vector* Hparasitic;

    // Loop Transfer Functions
    struct vector* Hloop;
    struct vector* Hclosedloop;
    struct vector* Hclosedloop_denominator;
    struct vector* Stot;

    // Results (one each for both min_Kvco and max_Kvco)
    double Jrms[2];
    double Jrms_vco[2];
    double Jrms_ref[2];
    double Jrms_cp[2];
    double Jrms_filter[2];
    double f0dB[2];
    double phasemargin[2];
    double fbw[2];
};

struct pll_state* pll_create(void)
{
    struct pll_state* state = malloc(sizeof(*state));
    state->numparpoles = 0;
    return state;
}

void pll_initialize(struct pll_state* state)
{
    unsigned int samples = (state->fupperexp - state->flowerexp) * state->pointsperdecade;
    state->f = vector_logspace(state->flowerexp, state->fupperexp, samples);
    state->s = vector_copy(state->f);
    vector_scale(state->s, 2 * CONSTANTS_PI * CONSTANTS_I);
    state->Hvco = vector_create(samples, 0);
    state->Hdetector = vector_create(samples, 0);
    state->Hcp = vector_create(samples, 0);
    state->Hparasitic = vector_create(samples, 0);
    state->Hloop = vector_create(samples, 0);
    state->Hclosedloop = vector_create(samples, 0);
    state->Hclosedloop_denominator = vector_create(samples, 0);

    state->Stot = vector_create(samples, 0);
    state->Stot_ref = vector_create(samples, 0);
    state->Stot_vco = vector_create(samples, 0);
    state->Stot_cp = vector_create(samples, 0);
    state->Stot_phasedetector = vector_create(samples, 0);
    state->Stot_filter = vector_create(samples, 0);
    state->Sref = vector_create(samples, 0);
    state->Svco = vector_create(samples, 0);
    state->Scp = vector_create(samples, 0);
    state->Sphasedetector = vector_create(samples, 0);
    state->Sfilter = vector_create(samples, 0);
    state->Nref = vector_create(samples, 0);
    state->Nvco = vector_create(samples, 0);
    state->Ncp = vector_create(samples, 0);
    state->Nphasedetector = vector_create(samples, 0);
    state->Nfilter = vector_create(samples, 0);
}

void pll_cleanup(struct pll_state* state)
{
    vector_destroy(state->f);
    vector_destroy(state->s);
    vector_destroy(state->Hvco);
    vector_destroy(state->Hcp);
    vector_destroy(state->Hparasitic);
    vector_destroy(state->Hloop);
    vector_destroy(state->Hdetector);
    vector_destroy(state->Hclosedloop);
    vector_destroy(state->Hclosedloop_denominator);
    vector_destroy(state->Stot);
    vector_destroy(state->Stot_ref);
    vector_destroy(state->Stot_vco);
    vector_destroy(state->Stot_cp);
    vector_destroy(state->Stot_phasedetector);
    vector_destroy(state->Stot_filter);
    vector_destroy(state->Sref);
    vector_destroy(state->Svco);
    vector_destroy(state->Scp);
    vector_destroy(state->Sphasedetector);
    vector_destroy(state->Sfilter);
    vector_destroy(state->Nref);
    vector_destroy(state->Nvco);
    vector_destroy(state->Ncp);
    vector_destroy(state->Nphasedetector);
    vector_destroy(state->Nfilter);
    free(state);
}

void pll_set_eval_frequencies(struct pll_state* state, int flowerexp, int fupperexp, unsigned int pointsperdecade)
{
    state->flowerexp = flowerexp;
    state->fupperexp = fupperexp;
    state->pointsperdecade = pointsperdecade;
}

void pll_set_input_output_frequencies(struct pll_state* state, double fref, double fsig)
{
    state->fref = fref;
    state->fsig = fsig;
}

void pll_set_feedback_divider(struct pll_state* state, unsigned int factor)
{
    state->N = factor;
}

void pll_set_reference_divider(struct pll_state* state, unsigned int factor)
{
    state->M = factor;
}

void pll_add_parasitic_pole(struct pll_state* state, double pole)
{
    if(state->numparpoles == 0)
    {
        state->parpoles = malloc(1 * sizeof(*state->parpoles));
    }
    else
    {
        state->parpoles = realloc(state->parpoles, (state->numparpoles + 1) * sizeof(*state->parpoles));
    }
    state->parpoles[state->numparpoles] = pole;
    ++state->numparpoles;
}

void pll_set_phase_detector_gain(struct pll_state* state, double gain)
{
    state->detectorgain = gain;
}

void pll_set_phase_detector_noise(struct pll_state* state, double S0)
{
    state->Sphasedetector0 = S0;
}

void pll_set_vco_gain(struct pll_state* state, double min_Kvco, double max_Kvco)
{
    state->min_Kvco = min_Kvco;
    state->max_Kvco = max_Kvco;
}

void pll_set_vco_noise(struct pll_state* state, double f0, double L0, double fc)
{
    state->dfvco0 = f0;
    state->Svco0 = noise_L_to_S(L0);
    state->dfvco0fc = fc;
}

void pll_set_reference_noise(struct pll_state* state, double f0, double L0, double fc)
{
    state->dfref0 = f0;
    state->Sref0 = noise_L_to_S(L0);
    state->dfref0fc = fc;
}

void pll_set_filter(struct pll_state* state, double Rf, double Cf, double Cfx)
{
    state->Rf = Rf;
    state->Cf = Cf;
    state->Cfx = Cfx;
}

void pll_set_chargepump_gain(struct pll_state* state, double gm)
{
    state->gm = gm;
}

void pll_set_chargepump_noise(struct pll_state* state, double S0, double fc)
{
    state->Scp0 = S0;
    state->fccp = fc;
}

static void _effective_noise(struct vector* result, const struct vector* NTF, const struct vector* S)
{
    // result = abs(NTF)^2 * S
    vector_copy_values(result, NTF);
    vector_abs_squared(result);
    vector_multiply(result, S);
}

static struct vector* _make_1pole_tf(const struct vector* s, complex double pole)
{
    struct vector* tfden = vector_copy(s);
    vector_scale(tfden, -1 / (2 * CONSTANTS_PI * pole));
    vector_add_scalar(tfden, 1);
    struct vector* tf = vector_copy(s);
    vector_set_all(tf, 1);
    vector_divide(tf, tfden);
    vector_destroy(tfden);
    return tf;
}

int pll_calculate(struct pll_state* state)
{
    unsigned int k = state->fsig / state->fref; // multiple between input and output

    for(size_t i = 0; i < 2; ++i)
    {
        /*
         * Loop Gain Transfer Functions *
         */
        
        // Hvco: 2 * pi * Kvco / s
        double Kvco;
        if(i == 0)
        {
            Kvco = state->min_Kvco;
        }
        else
        {
            Kvco = state->max_Kvco;
        }
        vector_set_all(state->Hvco, 2 * CONSTANTS_PI * Kvco);
        vector_divide(state->Hvco, state->s);

        // Hfilter (1 + s * Cf * Rf) / (s * (Cf + Cfx) + s * s * Rf * Cf * Cfx)
        struct vector* Hfilter_denominator1 = vector_copy(state->s);
        vector_scale(Hfilter_denominator1, state->Cf + state->Cfx);
        struct vector* Hfilter_denominator = vector_copy(state->s);
        vector_multiply(Hfilter_denominator, state->s);
        vector_scale(Hfilter_denominator, state->Rf * state->Cf * state->Cfx);
        vector_add(Hfilter_denominator, Hfilter_denominator1);
        struct vector* Hfilter = vector_copy(state->s);
        vector_scale(Hfilter, state->Rf * state->Cf);
        vector_add_scalar(Hfilter, 1);
        vector_divide(Hfilter, Hfilter_denominator);

        // Hparasitic
        vector_set_all(state->Hparasitic, 1);
        for(size_t i = 0; i < sizeof(state->parpoles) / sizeof(state->parpoles[0]); ++i)
        {
            struct vector* tf = _make_1pole_tf(state->s, state->parpoles[i]);
            vector_multiply(state->Hparasitic, tf);
            vector_destroy(tf);
        }

        // Hdetector: detectorgain (poles are included in Hparasitic)
        vector_set_all(state->Hdetector, state->detectorgain);

        // Hcp: gm (poles are included in Hparasitic)
        vector_set_all(state->Hcp, state->gm);

        // Hloop: Hdetector * Hcp * Hfilter * Hvco * Hparasitic
        vector_set_all(state->Hloop, 1);
        vector_multiply(state->Hloop, state->Hdetector);
        vector_multiply(state->Hloop, state->Hcp);
        vector_multiply(state->Hloop, Hfilter);
        vector_multiply(state->Hloop, state->Hvco);
        vector_multiply(state->Hloop, state->Hparasitic);

        // Hclosedloop: Hloop / (1 + 1 / N * Hloop)
        vector_copy_values(state->Hclosedloop_denominator, state->Hloop);
        vector_scale(state->Hclosedloop_denominator, 1.0 / state->N);
        vector_add_scalar(state->Hclosedloop_denominator, 1);
        vector_copy_values(state->Hclosedloop, state->Hloop);
        vector_divide(state->Hclosedloop, state->Hclosedloop_denominator);

        /*
         * Noise Transfer Functions and Effective Noise Contributions (Power Spectral Densities) *
         */

        // Reference
        vector_set_all(state->Sref, 0);
        noise_PSD_20dB_per_decade(state->Sref, state->f, state->dfref0, state->Sref0);
        noise_PSD_30dB_per_decade(state->Sref, state->f, state->dfref0fc, state->dfref0 / state->dfref0fc * state->Sref0);
        // Nref: k / M * Hclosedloop
        state->Nref = vector_copy(state->Hclosedloop);
        vector_scale(state->Nref, k / state->M);
        _effective_noise(state->Stot_ref, state->Nref, state->Sref);

        // VCO
        vector_set_all(state->Svco, 0);
        noise_PSD_20dB_per_decade(state->Svco, state->f, state->dfvco0, state->Svco0);
        noise_PSD_30dB_per_decade(state->Svco, state->f, state->dfvco0fc, state->dfvco0 / state->dfvco0fc * state->Svco0);
        // Nvco: 1 / (1 + 1 / N * Hloop)
        vector_set_all(state->Nvco, 1);
        vector_divide(state->Nvco, state->Hclosedloop_denominator);
        _effective_noise(state->Stot_vco, state->Nvco, state->Svco);


        // Charge Pump
        vector_set_all(state->Scp, 0);
        noise_PSD_white_flicker(state->Scp, state->f, state->Scp0, state->fccp);
        // Ncp: Hclosedloop / (detectorgain * gm)
        vector_copy_values(state->Ncp, state->Hclosedloop);
        vector_scale(state->Ncp, 1 / (state->detectorgain * state->gm));
        _effective_noise(state->Stot_cp, state->Ncp, state->Scp);

        // Phase Detector
        vector_set_all(state->Sphasedetector, state->Sphasedetector0);
        // Nphasedetector: gm * Hfilter * Hvco / (1 + 1 / N * Hloop)
        vector_set_all(state->Nphasedetector, 1);
        vector_scale(state->Nphasedetector, state->gm);
        vector_multiply(state->Nphasedetector, Hfilter);
        vector_multiply(state->Nphasedetector, state->Hvco);
        vector_divide(state->Nphasedetector, state->Hclosedloop_denominator);
        _effective_noise(state->Stot_phasedetector, state->Nphasedetector, state->Sphasedetector);

        // Filter
        double Sfilter0 = 1.657e-20 * state->Rf;
        vector_set_all(state->Sfilter, 0);
        noise_PSD_constant(state->Sfilter, state->f, Sfilter0);
        // Nfilter: 1 / (detectorgain * gm * Hfilter) * Hclosedloop
        vector_copy_values(state->Nfilter, state->Hclosedloop);
        vector_scale(state->Nfilter, 1 / (state->detectorgain * state->gm));
        vector_divide(state->Nfilter, Hfilter);
        _effective_noise(state->Stot_filter, state->Nfilter, state->Sfilter);

        // Total PLL Noise
        vector_set_all(state->Stot, 0);
        vector_add(state->Stot, state->Stot_ref);
        vector_add(state->Stot, state->Stot_vco);
        vector_add(state->Stot, state->Stot_cp);
        vector_add(state->Stot, state->Stot_phasedetector);
        vector_add(state->Stot, state->Stot_filter);

        state->Jrms[i] = noise_RMSjitterS(state->fsig, state->f, state->Stot);
        transfer_unity_gain_frequency(state->f, state->Hloop, &state->f0dB[i]);
        transfer_phase_margin(state->f, state->Hloop, &state->phasemargin[i]);
        transfer_lowpass_bandwidth(state->f, state->Hloop, &state->fbw[i]);
        
        // integrated jitter contributions (FIXME: is this really correct? Does this need a sqrt somewhere?)
        double Atot = noise_trapzS(state->f, state->Stot);
        state->Jrms_vco[i] = state->Jrms[i] * noise_trapzS(state->f, state->Stot_vco) / Atot;
        state->Jrms_ref[i] = state->Jrms[i] * noise_trapzS(state->f, state->Stot_ref) / Atot;
        state->Jrms_cp[i] = state->Jrms[i] * noise_trapzS(state->f, state->Stot_cp) / Atot;
        state->Jrms_filter[i] = state->Jrms[i] * noise_trapzS(state->f, state->Stot_filter) / Atot;
    }

    return 1;
}

double pll_get_score(struct pll_state* state, evaluator eval)
{
    return eval(state->phasemargin[0], state->fbw[0], state->Jrms[0]);
}

void pll_print_result(struct pll_state* state)
{
    for(size_t i = 0; i < 2; ++i)
    {
        if(i == 0)
        {
            printf("%s\n", "* Results with Minimum Kvco *");
        }
        else
        {

            printf("%s\n", "* Results with Maximum Kvco *");
        }
        char* Jrms_formatted = engineering_format(state->Jrms[i], "s", 1);
        char* Jrms_vco_formatted = engineering_format(state->Jrms_vco[i], "s", 1);
        char* Jrms_ref_formatted = engineering_format(state->Jrms_ref[i], "s", 1);
        char* Jrms_cp_formatted = engineering_format(state->Jrms_cp[i], "s", 1);
        char* Jrms_filter_formatted = engineering_format(state->Jrms_filter[i], "s", 1);
        char* f0dB_formatted = engineering_format(state->f0dB[i], "Hz", 1);
        char* phasemargin_formatted = engineering_format(state->phasemargin[i], "Degree", 1);
        char* fbw_formatted = engineering_format(state->fbw[i], "Hz", 1);
        printf("Jrms        = %s\n", Jrms_formatted);
        printf("Jrms vco    = %s (%.1f %%)\n", Jrms_vco_formatted, 100 * state->Jrms_vco[i] / state->Jrms[i]);
        printf("Jrms ref    = %s (%.1f %%)\n", Jrms_ref_formatted, 100 * state->Jrms_ref[i] / state->Jrms[i]);
        printf("Jrms cp     = %s (%.1f %%)\n", Jrms_cp_formatted, 100 * state->Jrms_cp[i] / state->Jrms[i]);
        printf("Jrms filter = %s (%.1f %%)\n", Jrms_filter_formatted, 100 * state->Jrms_filter[i] / state->Jrms[i]);
        printf("f0dB = %s\n", f0dB_formatted);
        printf("phase margin = %s\n", phasemargin_formatted);
        printf("fbw = %s\n", fbw_formatted);
        printf("%s\n", "*****************************");
        free(Jrms_formatted);
        free(Jrms_vco_formatted);
        free(f0dB_formatted);
        free(phasemargin_formatted);
        free(fbw_formatted);

    }
}

