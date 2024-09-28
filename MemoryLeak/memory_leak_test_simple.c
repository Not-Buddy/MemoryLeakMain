#include <stdio.h>
#include <stdlib.h>

void test_leaks() {
    // Allocate memory without freeing (leak)
    int *arr1 = (int*)malloc(10 * sizeof(int));
    
    // Allocate and free (no leak)
    int *arr2 = (int*)malloc(20 * sizeof(int));
    free(arr2);
}

int main() {
    test_leaks();
    printf("Program ran without memory tracking.\n");
    return 0;
}

