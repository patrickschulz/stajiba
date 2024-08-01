#ifndef PLL_PARAMETER
#define PLL_PARAMETER

struct parameter;
struct parameter* parameter_create(double start, double end, double step);
double parameter_next(struct parameter* parameter);
void parameter_reset(struct parameter* parameter);
int parameter_finished(struct parameter* parameter);

#endif /* PLL_PARAMETER */
