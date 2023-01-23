//
// Created by Andrew Bowie on 1/19/23.
//

#include "math.h"

unsigned int ui_realmod(int x, int mod)
{
        int val = x % mod;
        return val < 0 ? val + mod : val;
}
double pow(double a, double b) {
        double c = a;
        
        int i = 1;
        if (b != 0) {
                if (i < b) {
                int i = 1;
                while (i < b) {
        c = c * a;
        i++;
                        }
}
        if (i > b) {
        int i = -1;
        while (i > b) {
        c = c * a;
        i--;
        }
return (1 / c);
}
return c;
}
return 1;
}