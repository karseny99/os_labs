#include <math.h>
#include <iostream>


float Pi1(int k) {
    float pi = 0;
    float cur = 1;
    float sign = 1;
    
    for(int i = 0; i < k; ++i) {
        pi += (1 / cur) * sign;
        cur += 2;
        sign *= -1;
    }

    return pi * 4;
}

float Pi2(int k) {
    float pi = 1;

    for(float i = 1; i <= k; ++i) {
        pi *= ((2 * i) * (2 * i)) / ((2 * i - 1) * (2 * i + 1));
    }

    return pi * 2;
}