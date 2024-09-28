#include "memory_leak_detector.h"
#include <stdio.h>

void test_with_leak() {
    int* arr = (int*)malloc(5 * sizeof(int));
    // Intentionally not freeing 'arr' to create a memory leak
}

void test_without_leak() {
    char* str = (char*)malloc(20 * sizeof(char));
    free(str);
}

int main() {
    init_memory_leak_detector();

    printf("Running tests...\n");

    test_with_leak();
    test_without_leak();

    printf("Tests completed. Checking for memory leaks:\n");
    print_memory_leaks();

    return 0;
}
