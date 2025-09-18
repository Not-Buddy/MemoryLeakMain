#ifndef MEMORY_LEAK_DETECTOR_H
#define MEMORY_LEAK_DETECTOR_H

#include <stdio.h>
#include <stdlib.h>

// Function to initialize the memory leak detector
void init_memory_leak_detector();

// Function to allocate memory and track it
void* tracked_malloc(size_t size, const char* file, int line);

// Function to free memory and remove it from tracking
void tracked_free(void* ptr, const char* file, int line);

// Function to print any memory leaks
void print_memory_leaks();

// Only define these macros if we're not in the implementation file
#ifndef MEMORY_LEAK_DETECTOR_IMPL
#define malloc(size) tracked_malloc(size, __FILE__, __LINE__)
#define free(ptr) tracked_free(ptr, __FILE__, __LINE__)
#endif

#endif // MEMORY_LEAK_DETECTOR_H
