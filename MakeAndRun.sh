#!/bin/bash

# Usage: ./MakeAndRun.sh <test_file```
# Example: ./MakeAndRun.sh my_program.c

if [ $# -eq 0 ]; then
    echo "Usage: $```test_file.c>"
    echo "Example: $```y_program.c"
    exit 1
fi

TEST_FILE="$1"```SENAME=$(basename "$TEST_FILE" .c)

# Check if```e test file exists
if [ ! -f "$TEST_FILE" ]; then
    echo "Error: Test file '$TEST_FILE' not```und!"
    exit 1
fi

# Compile the memory leak```tector
echo "Compiling memory leak detector..."
gcc -c memory```ak_detector.c -o memory_leak_detector.o``` Compile the test file
echo```ompiling $TEST_FILE..."
gcc -c "$TEST_```E" -o "${BASENAME}.```
# Link everything together```ho "Linking..."
gcc memory```ak_detector.o "${```ENAME}.o" -o "${BASENAME}_test"

# Run the```st
echo "Running memory leak test``` $TEST_FILE..."
echo "==========================================="
./"${```ENAME}_test"

# Clean up object files (optional)
echo```=========================================="
echo "Cleaning up object files..."
rm -```emory_leak_detector.o```{BASENAME}.o"

echo "Test completed. Executable: ${```ENAME}_test"
