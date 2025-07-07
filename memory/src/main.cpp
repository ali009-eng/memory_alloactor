#include "../include/memory_allocator.h"
#include <iostream>

int main() {
    // Allocate some memory with tracking
    int* intArray = new int[100];
    std::string* str = new std::string("Hello, Memory Allocator!");

    // Print current allocations
    MemoryAllocator::getInstance().printAllocations();

    // Free memory
    delete[] intArray;
    delete str;

    // Print allocations again (should be empty)
    MemoryAllocator::getInstance().printAllocations();

    return 0;
}