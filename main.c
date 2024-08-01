#include <stdio.h>

#include <math.h>
#include <float.h>

#include "parameter.h"
#include "pll.h"

double eval(double phasemargin, double bandwidth, double Jrms)
{
    // phase margin should be between 60 and 80
    // this target fails (0) when it is not
    double phasemargin_target;
    //if(phasemargin < 60 || phasemargin > 80)
    if(0)
    {
        phasemargin_target = DBL_MAX; // fail
    }
    else
    {
        phasemargin_target = 10 * fabs(phasemargin - 70.0);
    }

    // the bandwidth can be low, but it must not be higher than 50 MHz
    double bandwidth_target;
    if(bandwidth > 2e6)
    {
        bandwidth_target = DBL_MAX; // fail
    }
    else
    {
        bandwidth_target = 0.0;
    }

    // the jitter should be minimized, but a fail target can be set
    // in order to be subject to the optimization,
    // the jitter values are scaled to a sensible range
    double Jrms_target = Jrms / 50e-15;
    //double score = phasemargin_target + bandwidth_target + 0 * Jrms_target;
    //double score = phasemargin_target + bandwidth_target;
    double score = phasemargin_target;
    //printf("Jrms: %5.1f fs\n", Jrms / 1e-15);
    //printf("phase margin: %.1f degree\n", phasemargin);
    //printf("bandwidth: %.1f MHz\n", bandwidth / 1e6);
    //printf("score: %g \n", score);
    //putchar('\n');
    return score;
}

int main()
{
    struct pll_state* pll_state = pll_create();

    // 10^3 <-> 10^12 with 50 points per decade
    pll_set_eval_frequencies(pll_state, 3, 12, 10);

    // reference and output frequency
    pll_set_input_output_frequencies(pll_state, 875e6, 56e9);

    // feedback divider
    pll_set_feedback_divider(pll_state, 1);

    // reference divider
    pll_set_reference_divider(pll_state, 1);

    // Sampler
    pll_set_phase_detector_gain(pll_state, 0.45);

    // oscillator: 0.2 GHz/V - 1.2 GHz/V; -90.0 dBc @ 1 MHz with 100 kHz flicker noise corner
    pll_set_vco_gain(pll_state, 1.2e9, 1.2e9);
    pll_set_vco_noise(pll_state, 1e6, -90.0, 1e5);

    // Reference
    pll_set_reference_noise(pll_state, 1e3, -139, 1e-3);

    // parasitic poles (in Hz, not rad/s!)
    pll_add_parasitic_pole(pll_state, -1e10);
    pll_add_parasitic_pole(pll_state, -1e13);

    // Charge Pump
    pll_set_chargepump_gain(pll_state, 200e-6);
    pll_set_chargepump_noise(pll_state, 1e-22, 1e7);

    // Filter
    pll_set_filter(pll_state, 1.0e3, 200.0e-12, 0e-12);

    // targets
    double Jrms_target = 100e-15;
    double phasemargin_target = 60;
    double fbw_target = 20e6;

    // define parameter ranges for filter values
    struct parameter* Rf_parameter = parameter_create(100, 10e3, 100);
    struct parameter* Cf_parameter = parameter_create(20e-12, 200e-12, 10e-12);

    double last_score = DBL_MAX;

    // final filter values
    double Rfvalue = 100;
    double Cfvalue = 20e-12;

    pll_initialize(pll_state);

    // run optimization
    size_t numruns = 0;
    while(!parameter_finished(Cf_parameter))
    {
        double Cf = parameter_next(Cf_parameter);
        while(!parameter_finished(Rf_parameter))
        {
            double Rf = parameter_next(Rf_parameter);
            pll_set_filter(pll_state, Rf, Cf, 0e-12);
            //printf("Rf = %g, Cf = %g\n", Rf, Cf);
            int valid = pll_calculate(pll_state);
            ++numruns;
            if(valid)
            {
                double score = pll_get_score(pll_state, eval);
                if(score < last_score)
                {
                    last_score = score;
                    Rfvalue = Rf;
                    Cfvalue = Cf;
                }
            }
        }
        parameter_reset(Rf_parameter);
    }

    // final run to print results
    pll_set_filter(pll_state, Rfvalue, Cfvalue, 0e-12);
    int valid = pll_calculate(pll_state);
    if(valid)
    {
        printf("completed after %zd runs\n", numruns);
        printf("Rf = %.1f Ohm, Cf = %.1f pF\n\n", Rfvalue, Cfvalue / 1e-12);
        pll_print_result(pll_state);
    }

    pll_cleanup(pll_state);
}
