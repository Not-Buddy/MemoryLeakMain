#!/bin/bash

# Memory Leak Testing Script
# Usage: ./MakeAndRun.sh <combined_sched.c>

if [ $# -eq 0 ]; then
    echo "Usage: $0 <combined_sched.c>"
    echo "Example: $0 combined_sched.c"
    exit 1
fi

TEST_FILE="$1"
BASENAME=$(basename "$TEST_FILE" .c)

echo "=== Memory Leak Testing Setup ==="
echo "Test file: $TEST_FILE"
echo ""

# Check if required files exist
echo "Checking required files..."
MISSING_FILES=0

if [ ! -f "$TEST_FILE" ]; then
    echo "❌ $TEST_FILE not found!"
    MISSING_FILES=1
fi

if [ ! -f "memory_leak_detector.c" ]; then
    echo "❌ memory_leak_detector.c not found!"
    MISSING_FILES=1
else
    echo "✓ memory_leak_detector.c found"
fi

if [ ! -f "memory_leak_detector_proper.h" ]; then
    echo "❌ memory_leak_detector_proper.h not found!"
    MISSING_FILES=1
else
    echo "✓ memory_leak_detector_proper.h found"
fi

if [ $MISSING_FILES -eq 1 ]; then
    echo ""
    echo "Missing required files. Please make sure all files are present."
    exit 1
fi

# Create a temporary version of combined_sched.c with correct header
echo ""
echo "Step 1: Preparing source file with correct headers..."
sed 's/#include "memory_leak_detector.h"/#include "memory_leak_detector_proper.h"/' "$TEST_FILE" > temp_"$TEST_FILE"

# Compile memory leak detector
echo ""
echo "Step 2: Compiling memory leak detector..."
if gcc -c memory_leak_detector.c -o memory_leak_detector.o; then
    echo "✓ Memory leak detector compiled"
else
    echo "❌ Failed to compile memory leak detector"
    rm -f temp_"$TEST_FILE"
    exit 1
fi

# Compile the test file
echo ""
echo "Step 3: Compiling $TEST_FILE..."
if gcc -c temp_"$TEST_FILE" -o "${BASENAME}.o" -lm; then
    echo "✓ Test file compiled"
else
    echo "❌ Failed to compile test file"
    rm -f temp_"$TEST_FILE" memory_leak_detector.o
    exit 1
fi

# Link everything
echo ""
echo "Step 4: Linking..."
if gcc memory_leak_detector.o "${BASENAME}.o" -o "${BASENAME}_test" -lm; then
    echo "✓ Linking successful"
else
    echo "❌ Linking failed"
    rm -f temp_"$TEST_FILE" memory_leak_detector.o "${BASENAME}.o"
    exit 1
fi

# Create a simple test taskset if it doesn't exist
if [ ! -f "test_taskset.txt" ]; then
    echo ""
    echo "Creating test taskset file..."
    cat > test_taskset.txt << EOF
3
0 10 3 10
0 15 4 15  
0 20 2 20
EOF
    echo "✓ Created test_taskset.txt"
fi

# Run the test
echo ""
echo "Step 5: Running memory leak test..."
echo "==========================================="
if [ -f "${BASENAME}_test" ]; then
    ./"${BASENAME}_test" test_taskset.txt
    EXIT_CODE=$?
else
    echo "❌ Executable not found: ${BASENAME}_test"
    exit 1
fi

# Clean up temporary and object files
echo ""
echo "==========================================="
echo "Cleaning up..."
rm -f temp_"$TEST_FILE" memory_leak_detector.o "${BASENAME}.o"

echo ""
if [ $EXIT_CODE -eq 0 ]; then
    echo "✓ Test completed successfully!"
    echo "Executable saved as: ${BASENAME}_test"
    echo ""
    echo "You can run it again with:"
    echo "./${BASENAME}_test test_taskset.txt"
else
    echo "⚠️  Test completed with exit code: $EXIT_CODE"
fi
