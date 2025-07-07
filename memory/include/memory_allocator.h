#ifndef MEMORY_ALLOCATOR_H
#define MEMORY_ALLOCATOR_H

#include <string>
#include <unordered_map>
#include <vector>
#include <iostream>
#include <cstdlib>

struct AllocationInfo {
    void* ptr;
    size_t size;
    std::string filename;
    int line;
};

class MemoryAllocator {
public:
    // Singleton pattern to ensure one instance
    static MemoryAllocator& getInstance() {
        static MemoryAllocator instance;
        return instance;
    }

    // Allocate memory with tracking
    void* allocate(size_t size, const std::string& filename, int line);

    // Deallocate memory
    void deallocate(void* ptr);

    // Print all current allocations
    void printAllocations() const;

    // Get total allocated memory
    size_t getTotalAllocated() const { return totalAllocated; }

    // Disable copy and assignment
    MemoryAllocator(const MemoryAllocator&) = delete;
    MemoryAllocator& operator=(const MemoryAllocator&) = delete;

private:
    MemoryAllocator(); // Private constructor for singleton
    ~MemoryAllocator();

    // Memory block structure
    struct MemoryBlock {
        void* memory;
        size_t size;
        bool used;
    };

    // Allocate a new block of memory from the system
    void allocateNewBlock(size_t size);

    // Find a free block of at least 'size' bytes
    MemoryBlock* findFreeBlock(size_t size);

    // Split a block if it's larger than needed
    void splitBlock(MemoryBlock* block, size_t size);

    // Merge adjacent free blocks
    void mergeFreeBlocks();

    std::vector<MemoryBlock> blocks;
    std::unordered_map<void*, AllocationInfo> allocationMap;
    size_t totalAllocated;
    size_t blockIncrement; // How much to grow memory by when needed
};

// Overloads for tracking allocations
inline void* operator new(size_t size, const std::string& filename, int line) {
    return MemoryAllocator::getInstance().allocate(size, filename, line);
}

inline void* operator new[](size_t size, const std::string& filename, int line) {
    return MemoryAllocator::getInstance().allocate(size, filename, line);
}

inline void operator delete(void* ptr) noexcept {
    MemoryAllocator::getInstance().deallocate(ptr);
}

inline void operator delete[](void* ptr) noexcept {
    MemoryAllocator::getInstance().deallocate(ptr);
}

// Only define the new macro if we're not in a system header
#ifndef __GLIBCXX__
#define new new(__FILE__, __LINE__)
#endif

#endif // MEMORY_ALLOCATOR_H