/**
 * Interface for systems-specific functionality, such as 
 * measuring energy consumption on CPU/GPU.
 */
#pragma once
#include <cstdint>

typedef uint32_t joule_t;

joule_t _get_CPU_joules();
joule_t _get_GPU_joules();