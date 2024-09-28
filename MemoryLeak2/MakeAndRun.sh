gcc -c memory_leak_detector.c -o memory_leak_detector.o
gcc -c test_memory_leak.c -o test_memory_leak.o
gcc memory_leak_detector.o test_memory_leak.o -o memory_leak_test
./memory_leak_test
