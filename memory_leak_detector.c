#define MEMORY_LEAK_DETECTOR_IMPL
#include "memory_leak_detector.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_ALLOCATIONS 1000

typedef struct {
    void* ptr;
    size_t size;
    const char* file;
    int line;
} Allocation;

static Allocation allocations[MAX_ALLOCATIONS];
static int allocation_count = 0;

void init_memory_leak_detector() {
    allocation_count = 0;
}

void* tracked_malloc(size_t size, const char* file, int line) {
    void* ptr = malloc(size);
    if (ptr) {
        if (allocation_count < MAX_ALLOCATIONS) {
            allocations[allocation_count].ptr = ptr;
            allocations[allocation_count].size = size;
            allocations[allocation_count].file = file;
            allocations[allocation_count].line = line;
            allocation_count++;
        } else {
            printf("Warning: Too many allocations to track!\n");
        }
    }
    return ptr;
}

void tracked_free(void* ptr, const char* file, int line) {
    if (ptr) {
        for (int i = 0; i < allocation_count; i++) {
            if (allocations[i].ptr == ptr) {
                allocations[i] = allocations[allocation_count - 1];
                allocation_count--;
                free(ptr);
                return;
            }
        }
        printf("Warning: Attempt to free untracked memory at %p in %s, line %d\n", ptr, file, line);
    }
    free(ptr);
}

void print_memory_leaks() {
    if (allocation_count == 0) {
        printf("No memory leaks detected.\n");
    } else {
        printf("Memory leaks detected:\n");
        for (int i = 0; i < allocation_count; i++) {
            printf("Leak: %zu bytes at %p, allocated in %s, line %d\n",
                   allocations[i].size, allocations[i].ptr,
                   allocations[i].file, allocations[i].line);
        }
    }
}
