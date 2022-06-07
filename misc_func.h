/*
misc_func.h
function prototypes for miscellaneous functions used in bgc routines
for use with pointbgc front-end to BBGC MuSo v3.0.6 library

*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
BBGC MuSo v3.0.8
Copyright 2000, Peter E. Thornton
Numerical Terradynamics Simulation Group
*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
*/
/* function prototypes for smoothing functions */
int run_avg(const double *input, double *output, int n, int w, int w_flag);
int boxcar_smooth(double* input, double* output, int n, int w, int w_flag);
