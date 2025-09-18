#ifndef MEMORY_LEAK_WRAPPER_H
#define MEMORY_LEAK_WRAPPER_H

#include "memory_leak_detector.h"

// Convenience macros to add memory leak detection
#define START_MEMORY_LEAK_DETECTION() \
    do { \
        init_memory_leak_detector(); \
        printf("=== Memory Leak Detection Started ===\n"); \
    } while(0)

#define END_MEMORY_LEAK_DETECTION() \
    do { \
        printf("=== Checking for Memory Leaks ===\n"); \
        print_memory_leaks(); \
        printf("=== Memory Leak Detection Completed ===\n"); \
    } while(0)

#endif // MEMORY_LEAK_WRAPPER_H
