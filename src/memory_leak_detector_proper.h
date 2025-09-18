#ifndef MEMORY_LEAK_DETECTOR_H
#define MEMORY_LEAK_DETECTOR_H

#include <stdio.h>
#include <stdlib.h>

// Function declarations
void init_memory_leak_detector();
void* tracked_malloc(size_t size, const char* file, int line);
void tracked_free(void* ptr, const char* file, int line);
void print_memory_leaks();

// Macro definitions
#ifndef MEMORY_LEAK_DETECTOR_IMPL
#define malloc(size) tracked_malloc(size, __FILE__, __LINE__)
#define free(ptr) tracked_free(ptr, __FILE__, __LINE__)
#endif

// Convenience macros
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

#endif // MEMORY_LEAK_DETECTOR_H
