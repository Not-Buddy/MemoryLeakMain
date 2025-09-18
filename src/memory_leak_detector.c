#define MEMORY_LEAK_DETECTOR_IMPL
#include "memory_leak_detector.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// Structure to track memory allocations
typedef struct MemoryBlock {
    void* ptr;
    size_t size;
    const char* file;
    int line;
    struct MemoryBlock* next;
} MemoryBlock;

static MemoryBlock* memory_list = NULL;
static int allocation_count = 0;
static size_t total_allocated = 0;

void init_memory_leak_detector() {
    memory_list = NULL;
    allocation_count = 0;
    total_allocated = 0;
    printf("Memory leak detector initialized\n");
}

void* tracked_malloc(size_t size, const char* file, int line) {
    void* ptr = malloc(size);
    if (ptr == NULL) {
        printf("Memory allocation failed at %s:%d\n", file, line);
        return NULL;
    }

    // Create a new memory block record
    MemoryBlock* block = (MemoryBlock*)malloc(sizeof(MemoryBlock));
    if (block == NULL) {
        // If we can't track it, still return the allocated memory
        return ptr;
    }

    block->ptr = ptr;
    block->size = size;
    block->file = file;
    block->line = line;
    block->next = memory_list;
    memory_list = block;

    allocation_count++;
    total_allocated += size;

    return ptr;
}

void tracked_free(void* ptr, const char* file, int line) {
    if (ptr == NULL) return;

    // Find and remove from tracking list
    MemoryBlock* current = memory_list;
    MemoryBlock* previous = NULL;

    while (current != NULL) {
        if (current->ptr == ptr) {
            // Found the block, remove it from list
            if (previous == NULL) {
                memory_list = current->next;
            } else {
                previous->next = current->next;
            }

            total_allocated -= current->size;
            allocation_count--;

            free(current); // Free the tracking block
            free(ptr);     // Free the actual memory
            return;
        }
        previous = current;
        current = current->next;
    }

    // If we reach here, the pointer wasn't found in our tracking list
    printf("Warning: Attempting to free untracked pointer at %s:%d\n", file, line);
    free(ptr);
}

void print_memory_leaks() {
    printf("\n=== MEMORY LEAK DETECTION RESULTS ===\n");

    if (memory_list == NULL) {
        printf("✓ NO MEMORY LEAKS DETECTED!\n");
        printf("All allocated memory was properly freed.\n");
    } else {
        printf("❌ MEMORY LEAKS DETECTED!\n");
        printf("The following memory blocks were not freed:\n");
        printf("\n%-20s %-10s %-10s %s\n", "Address", "Size", "Line", "File");
        printf("%-20s %-10s %-10s %s\n", "--------", "----", "----", "----");

        int leak_count = 0;
        size_t total_leaked = 0;
        MemoryBlock* current = memory_list;

        while (current != NULL) {
            printf("%-20p %-10zu %-10d %s\n", 
                   current->ptr, current->size, current->line, current->file);
            leak_count++;
            total_leaked += current->size;
            current = current->next;
        }

        printf("\nSUMMARY:\n");
        printf("Total leaked blocks: %d\n", leak_count);
        printf("Total leaked bytes: %zu\n", total_leaked);
    }

    printf("\nTotal allocations made: %d\n", allocation_count);
    printf("==========================================\n");
}
