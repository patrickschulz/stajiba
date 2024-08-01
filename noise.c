#include <math.h>

#include "vector.h"
#include "constants.h"

double noise_L_to_S(double L)
{
    return 2 * pow(10, L / 10);
}

static void _PSD_xxdB_per_decade(struct vector* S, struct vector* f, double df0, double S0, double exp)
{
    // S = S0 * (df0 / f)^exp
    for(size_t i = 0; i < vector_size(f); ++i)
    {
        double value = S0 * pow(df0 / vector_get(f, i), exp);
        vector_set(S, i, value + vector_get(S, i));
    }
}

void noise_PSD_constant(struct vector* S, struct vector* f, double S0)
{
    // use some non-zero dummy value for df0 to prevent 0^0
    _PSD_xxdB_per_decade(S, f, 1, S0, 0);
}

void noise_PSD_white_flicker(struct vector* S, struct vector* f, double Sfloor, double fc)
{
    _PSD_xxdB_per_decade(S, f, 1, Sfloor, 0);
    _PSD_xxdB_per_decade(S, f, fc, Sfloor, 1);
}

void noise_PSD_10dB_per_decade(struct vector* S, struct vector* f, double df0, double S0)
{
    _PSD_xxdB_per_decade(S, f, df0, S0, 1);
}

void noise_PSD_20dB_per_decade(struct vector* S, struct vector* f, double df0, double S0)
{
    _PSD_xxdB_per_decade(S, f, df0, S0, 2);
}

void noise_PSD_30dB_per_decade(struct vector* S, struct vector* f, double df0, double S0)
{
    _PSD_xxdB_per_decade(S, f, df0, S0, 3);
}

/*
function M.S_to_L(S)
    if getmetatable(S) == "vector" then
        return S:map(function(x) return 10 * math.log(0.5 * x, 10) end)
    elseif getmetatable(S) == "complex" then
        return 10 * math.log(0.5 * S:abs(), 10)
    else
        return 10 * math.log(0.5 * S, 10)
    end
end

-- this function is not a simple discrete integration of phase noise to obtain jitter
-- it linearly interpolates the data in dBc/Hz, since phase noise profiles are usually pretty linear in this domain
-- a straight-forward trapezoidal integration of the linear (W/Hz) data would not yield this accurately with only a few points
-- This method is taken from "Converting Oscillator Phase Noise to Time Jitter" by Walt Kester (Analog Devices MT-008 Tutorial)
function M.RMSjitterL(f0, f, L)
    local A = 0
    for i = 1, #f - 1 do
        local flower = f[i]
        local fupper = f[i + 1]
        local Llower = L[i]
        local Lupper = L[i + 1]
        local m = (Lupper - Llower) / (10 * math.log(fupper / flower, 10))
        A = A + 10 ^ (Llower / 10) / (m + 1) * flower * ((fupper / flower) ^ (m + 1) - 1)
    end
    local J = math.sqrt(2 * A) / (2 * math.pi * f0)
    return J
end
*/

// this function is not a simple discrete integration of phase noise to obtain the area under the curve (for jitter calculation)
// it linearly interpolates the data in dBc/Hz, since phase noise profiles are usually pretty linear in this domain
// a straight-forward trapezoidal integration of the linear (W/Hz) data would not yield this accurately with only a few points
// This method is taken from "Converting Oscillator Phase Noise to Time Jitter" by Walt Kester (Analog Devices MT-008 Tutorial)
// this function does operate on the linear 'S' (rad^2/Hz) data, not 'L' (dBc/Hz)
// the interpolation is still done on the logarithmic data
// the calculation was optimized so it looks a little different
double noise_trapzS(struct vector* f, struct vector* S)
{
    double A = 0;
    for(size_t i = 0; i < vector_size(f) - 1; ++i)
    {
        double Sval1 = creal(vector_get(S, i));
        double Sval2 = creal(vector_get(S, i + 1));
        double flower = vector_get(f, i);
        double fupper = vector_get(f, i + 1);
        double m = log(Sval2 / Sval1) / log(fupper / flower);
        A += 0.5 * Sval1 / (m + 1) * flower * (pow(fupper / flower, (m + 1)) - 1);
    }
    return A;
}

// this function yields the same results as RMSjitterL, but operates on linear data
// the interpolation is still done on the logarithmic data
// the calculation was optimized so it looks a little different
double noise_RMSjitterS(double f0, struct vector* f, struct vector* S)
{
    double A = noise_trapzS(f, S);
    double J = sqrt(2 * A) / (2 * CONSTANTS_PI * f0);
    return J;
}

/*
function M.trapz(x, y)
    local sum = 0
    for i = 1, #x - 1 do
        sum = sum + 0.5 * (y[i + 1] + y[i]) * (x[i + 1] - x[i])
    end
    return sum
end
*/
