//
// Created by Andrew Bowie on 1/19/23.
//

#include "math.h"

unsigned int ui_realmod(int x, int mod)
{
        int val = x % mod;
        return val < 0 ? val + mod : val;
}