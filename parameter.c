#include "parameter.h"

#include <stdlib.h>

struct parameter {
    double start;
    double end;
    double step;
    double current;
};

struct parameter* parameter_create(double start, double end, double step)
{
    struct parameter* parameter = malloc(sizeof(*parameter));
    parameter->start = start;
    parameter->end = end;
    parameter->step = step;
    parameter->current = start;
    return parameter;
}

int parameter_finished(struct parameter* parameter)
{
    return parameter->current > parameter->end;
}

void parameter_reset(struct parameter* parameter)
{
    parameter->current = parameter->start;
}

double parameter_next(struct parameter* parameter)
{
    double ret = parameter->current;
    parameter->current += parameter->step;
    return ret;
}

