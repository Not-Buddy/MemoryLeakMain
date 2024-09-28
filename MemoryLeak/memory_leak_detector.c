#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "memory_leak_detector.h"

// Struct to keep track of each allocation
typedef struct Allocation {
    void *ptr;
    size_t size;
    const char *file;
    int line;
    struct Allocation *next;
} Allocation;

static Allocation *allocations = NULL;

// Add allocation to list
void add_allocation(void *ptr, size_t size, const char *file, int line) {
    if (!ptr) return;  // Ignore NULL pointers

    Allocation *new_alloc = (Allocation*)malloc(sizeof(Allocation));
    if (!new_alloc) return;  // Allocation failed

    new_alloc->ptr = ptr;
    new_alloc->size = size;
    new_alloc->file = file;
    new_alloc->line = line;
    new_alloc->next = allocations;
    allocations = new_alloc;
}

// Remove allocation from list
void remove_allocation(void *ptr) {
    if (!ptr) return;  // Ignore NULL pointers

    Allocation *curr = allocations;
    Allocation *prev = NULL;

    while (curr) {
        if (curr->ptr == ptr) {
            if (prev) {
                prev->next = curr->next;
            } else {
                allocations = curr->next;  // Head of the list
            }
            free(curr);  // Free the tracking struct
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

// Custom malloc
void* custom_malloc(size_t size, const char *file, int line) {
    void *ptr = malloc(size);
    add_allocation(ptr, size, file, line);  // Add allocation tracking
    return ptr;
}

// Custom calloc
void* custom_calloc(size_t num, size_t size, const char *file, int line) {
    void *ptr = calloc(num, size);
    add_allocation(ptr, num * size, file, line);  // Add allocation tracking
    return ptr;
}

// Custom free
void custom_free(void *ptr, const char *file, int line) {
    remove_allocation(ptr);  // Remove tracking first
    free(ptr);  // Free the original memory
}

// Report leaks
void report_memory_leaks() {
    Allocation *curr = allocations;
    if (!curr) {
        printf("No memory leaks detected.\n");
        return;
    }

    printf("Memory leaks detected:\n");
    while (curr) {
        printf("Leaked %zu bytes at %p, allocated in %s:%d\n", curr->size, curr->ptr, curr->file, curr->line);
        curr = curr->next;
    }
}

