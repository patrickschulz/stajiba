#include <math.h>
#include <stdio.h>
#include <stdlib.h>

double fRand(double fMin, double fMax)
{
    double f = drand48();
    return fMin + f * (fMax - fMin);
}

int random_choice(double prob)
{
    double r = drand48();
    return r < prob;
}

int main()
{
    double temperature = 100000;
    double factor = 0.999;
    double R1 = 1000;
    double R2 = 10000; // initial value
    size_t iterations = 100;
    double last_voltage = R2 / (R1 + R2);
    double target = 0.5;
    size_t numsteps = 0;
    while(temperature > 0.000001)
    {
        for(size_t i = 0; i < iterations; ++i)
        {
            ++numsteps;
            double R2_ = R2 + fRand(-100, 100);
            double v = R2_ / (R1 + R2_);
            double delta = fabs(v - target) - fabs(last_voltage - target);
            if(delta < 0)
            {
                if(random_choice(exp(-delta / temperature)))
                {
                    R2 = R2_;
                    last_voltage = v;
                    printf("R2: %f, v = %.3f\n", R2, last_voltage);
                }
            }
        }
        temperature *= factor;
    }
    printf("number of steps: %zd\n", numsteps);
    printf("final R2: %f, v = %.3f\n", R2, last_voltage);
}
