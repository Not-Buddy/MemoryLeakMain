#ifndef MEMORY_LEAK_DETECTOR_H
#define MEMORY_LEAK_DETECTOR_H

#include <stddef.h> // for size_t

// Wrappers for memory allocation functions
void* custom_malloc(size_t size, const char *file, int line);
void* custom_calloc(size_t num, size_t size, const char *file, int line);
void custom_free(void *ptr, const char *file, int line);

// Function to report memory leaks
void report_memory_leaks();

#define malloc(size) custom_malloc(size, __FILE__, __LINE__)
#define calloc(num, size) custom_calloc(num, size, __FILE__, __LINE__)
#define free(ptr) custom_free(ptr, __FILE__, __LINE__)

#endif // MEMORY_LEAK_DETECTOR_H

