pkg load control;

% boltzman constant
kb = 1.380649e-23;

% settings
flowerexp = 3;
fupperexp = 10;
pointsperdecade = 10;
samples = (fupperexp - flowerexp) * pointsperdecade;
fref = 875e6;
fsig = 56e9;

k = fsig / fref; % multiple between input and output
N = 1; % feedback frequency divider factor (can be different from k for a SSPLL)
M = 1; % reference frequency divider factor

% Sampler
Rs = 20;
Cs = 5e-15;
Ssampler0 = 4 * kb * 300 * Rs;

% Charge Pump
gm = 200.0e-6;
Scp0 = 1e-22;
fccp = 1e7;

% oscillator
A0 = 0.45;
Kvco = 1.5e9;
Svco0 = 2 * 10^(-93.5 / 10);
dfvco0 = 1e6;
dfvco0fc = 1e6;

% Filter
Rf = 2.0e3;
Cf = 100.0e-12;
Cfx = 1e-12;
Sfilter0 = 1.657e-20 * Rf;

% Overall PLL Power Consumption
Pdc = 50e-3;

% parasitic poles (in Hz, not rad/s!)
%parpoles = [ -1e10, -1e13 ];
parpoles = [];

s = tf('s');

% Loop Gain Transfer Functions
Hvco = Kvco / s;
Hfilter = (1 + s * Rf * Cf) / (s * Cf);
Hfilter = (1 + s * Cf * Rf) / (s * (Cf + Cfx) + s^2 * Cf * Cfx * Rf)
Hparasitic = prod(parpoles) * tf(poly([]), poly(parpoles));
Hsampler = A0 * 1 / (1 + s * Rs * Cs);
Hcp = gm;
Hloop = Hsampler * Hcp * Hfilter * Hvco * Hparasitic;
Hclosedloop = (Hloop / (1 + 1 / N * Hloop));

it = figure;
bode(Hloop, Hloop / (1 + Hloop))

pause(0.05);
waitfor(it);

%% reference
%Srefdata = 2 * 10^(-139 / 10);
%local Sref = noise.PSD_20dB_per_decade(f, Srefdata.df0, Srefdata.S0)
%local Nref = 1 / (1 + 1 / N * Hloop)
%local Nref = k / M * Hclosedloop
%local Stot_ref = abs(Nref * Nref) * Sref
%
%-- VCO
%local Svco = noise.PSD_20dB_per_decade(f, dfvco0, Svco0) + noise.PSD_30dB_per_decade(f, dfvco0fc, dfvco0 / dfvco0fc * Svco0)
%local Nvco = 1 / (1 + 1 / N * Hloop)
%local Stot_vco = abs(Nvco * Nvco) * Svco
%
%-- Charge Pump
%local Scp = noise.PSD_white_flicker(f, Scp0, fccp)
%local Ncp = 1 / (A0 * gm) * Hclosedloop
%local Stot_cp = abs(Ncp * Ncp) * Scp
%
%-- Sampler
%local Ssampler = Ssampler0
%local Nsampler = gm * Hfilter * Hvco / (1 + 1 / N * Hloop)
%local Stot_sampler = abs(Nsampler * Nsampler) * Ssampler
%
%-- Filter
%local Sfilter = noise.PSD_constant(f, Sfilter0)
%local Nfilter = 1 / (A0 * gm * Hfilter) * Hclosedloop
%local Stot_filter = abs(Nfilter * Nfilter) * Sfilter
%
%-- Total Noise
%local Stot = Stot_ref + Stot_vco + Stot_cp + Stot_sampler + Stot_filter
%
%-- phase margin
%local f0dB = transfer.find_0dB(f, Hloop)
%local phasemargin = transfer.phasemargin(Hloop)
%
