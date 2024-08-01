# StaJiBa &ndash; A tool for PLL optimization
This repository provides work on a PLL optimization tool to find the best compromise between bandwidth, stability margins and integrated jitter.
With the inclusion of higher-order poles are a more complex loop controller, straight-forward calculation of filter coefficients and charge pump gain fails or becomes tedious. Therefore the best way to design PLL loop dynamics is by optimization.

The current state correctly implements PLL loop calculations and noise, but the optimization algorithm does not work very well and is sensitive to small changes.
