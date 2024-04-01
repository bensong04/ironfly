/**
 * "Mock" implementation of the Systems.hpp interface.
 * Returns random values for all functions.
 */
#include <random>
#include <cstdlib>
#include "Systems.hpp"

// LCG parameters for glibc rand()
#define A 1103515245
#define C 12345
#define M ((unsigned)(1 << 31))

#define max_of_type(type) (1 << sizeof(type))

// Use two distinct PRNGs
// Ensures random numbers do not "sync up"
auto CPU_rand_engine = std::linear_congruential_engine<
                                joule_t, A, C, M
                            >(1); // seed
auto GPU_rand_engine = std::linear_congruential_engine<
                                joule_t, A, C, M
                            >(2); // different seed

joule_t _get_CPU_joules() {
    // no need to seed
    return CPU_rand_engine() / RAND_MAX * max_of_type(joule_t);
}

joule_t _get_GPU_joules() {
    return GPU_rand_engine() / RAND_MAX * max_of_type(joule_t);
}