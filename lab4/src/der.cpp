#include <iostream>
#include <math.h>

float Derivative1(float A, float deltaX) {
    return (cos(A + deltaX) - cos(A)) / deltaX;
}

float Derivative2(float A, float deltaX) {
    return (cos(A + deltaX) - cos(A - deltaX)) / (2 * deltaX);
}
