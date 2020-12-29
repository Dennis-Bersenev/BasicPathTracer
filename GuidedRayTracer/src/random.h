#pragma once

#include <cstdlib>

inline double random_double() {
    return rand() / (RAND_MAX + 1.0);
}

inline int random_int(int min, int max) {
    // Returns a random integer in [min,max].
    int range = max - min + 1;
    return (rand() % range) + min;
    
}