//
// Created by Andrew Bowie on 1/19/23.
//

#include "math.h"

unsigned int ui_realmod(int x, int mod)
{
    int val = x % mod;
    return val < 0 ? val + mod : val;
}

double pow(double a, double b)
{
    double c = a;

    int i = 1;
    if (b != 0)
    {
        if (i < b)
        {
            int i = 1;
            while (i < b)
            {
                c = c * a;
                i++;
            }
        }
        if (i > b)
        {
            int i = -1;
            while (i > b)
            {
                c = c * a;
                i--;
            }
            return (1 / c);
        }
        return c;
    }
    return 1;
}

///The LCRNG multiplier from C++.
#define SEED_MULTI 214013
///The LCRNG addend from C++.
#define SEED_ADDEND 2531011

///The seed for random number generation.
static unsigned long rand_seed = 0L;

void s_rand(unsigned long seed)
{
    //Apply the initial scramble of the seed.
    rand_seed = (seed ^ SEED_MULTI);
}

unsigned int next_random(void)
{
    //Find the next seed.
    unsigned long next_seed = (rand_seed * SEED_MULTI + SEED_ADDEND);

    //Read 32 bits from the seed.
    unsigned int thing = (int) rand_seed >> 2;
    rand_seed = next_seed;
    return thing;
}