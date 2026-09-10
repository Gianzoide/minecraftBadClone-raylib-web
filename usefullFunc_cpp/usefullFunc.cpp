
#include <iostream>
#include <vector>
#include <list>
#include <algorithm>

#include <cmath>
#include <limits> // Required for numeric_limits

using namespace std;

int middle(int n) {
    return n / 2;
}

const float EPSILON = 0.00001f;

bool hasAllZerosAfterDot(float value) {
    // Compare the absolute difference between the value and its rounded version 
    // to the tolerance threshold.
    if (std::abs(std::round(value) - value) <= EPSILON) {
        return true; // The fractional part is effectively zero
    }
    else {
        return false; // There is a significant fractional part
    }
}