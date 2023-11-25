#include <iostream>
#include <math.h>
#include "../include/der.hpp"

extern "C" {
    float Der1(float A, float deltaX) {
        return (cos(A + deltaX) - cos(A)) / deltaX;
    }

    float Der2(float A, float deltaX) {
        return (cos(A + deltaX) - cos(A - deltaX)) / (2 * deltaX);
    }
}