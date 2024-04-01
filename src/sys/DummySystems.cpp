/**
 * "Mock" implementation of the Systems.hpp interface.
 * Returns constant values for all functions.
 */
#include "Systems.hpp"

joule_t _get_CPU_joules() {
    return 42;
}

joule_t _get_GPU_joules() {
    return 42;
}