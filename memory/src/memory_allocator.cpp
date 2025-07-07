#include "../include/memory_allocator.h"
#include <algorithm>

MemoryAllocator::MemoryAllocator() : totalAllocated(0), blockIncrement(1024 * 1024) {
    // Start with one block
    allocateNewBlock(blockIncrement);
}

MemoryAllocator::~MemoryAllocator() {
    // Print any leaks
    if (!allocationMap.empty()) {
        std::cerr << "Memory leaks detected:\n";
        printAllocations();
    }

    // Free all blocks
    for (auto& block : blocks) {
        free(block.memory);
    }
}

void* MemoryAllocator::allocate(size_t size, const std::string& filename, int line) {
    // Find a suitable block
    MemoryBlock* block = findFreeBlock(size);
    if (!block) {
        // No suitable block found, allocate more memory
        allocateNewBlock(std::max(size, blockIncrement));
        block = findFreeBlock(size);
        if (!block) {
            throw std::bad_alloc();
        }
    }

    // Mark the block as used
    block->used = true;

    // Track the allocation
    AllocationInfo info;
    info.ptr = block->memory;
    info.size = size;
    info.filename = filename;
    info.line = line;
    allocationMap[block->memory] = info;

    totalAllocated += size;
    return block->memory;
}

void MemoryAllocator::deallocate(void* ptr) {
    auto it = allocationMap.find(ptr);
    if (it == allocationMap.end()) {
        // Not our allocation or already freed
        return;
    }

    // Find the block containing this pointer
    for (auto& block : blocks) {
        if (block.memory == ptr) {
            block.used = false;
            totalAllocated -= it->second.size;
            allocationMap.erase(it);

            // Try to merge free blocks
            mergeFreeBlocks();
            return;
        }
    }

    // Pointer not found in any block (shouldn't happen)
    throw std::runtime_error("Attempt to free invalid pointer");
}

void MemoryAllocator::printAllocations() const {
    std::cout << "Current allocations (" << allocationMap.size() << "):\n";
    for (const auto& pair : allocationMap) {
        const AllocationInfo& info = pair.second;
        std::cout << "  " << info.ptr << " - " << info.size << " bytes allocated at "
                  << info.filename << ":" << info.line << "\n";
    }
    std::cout << "Total allocated: " << totalAllocated << " bytes\n";
}

void MemoryAllocator::allocateNewBlock(size_t size) {
    void* memory = malloc(size);
    if (!memory) {
        throw std::bad_alloc();
    }

    MemoryBlock newBlock;
    newBlock.memory = memory;
    newBlock.size = size;
    newBlock.used = false;
    blocks.push_back(newBlock);
}

MemoryAllocator::MemoryBlock* MemoryAllocator::findFreeBlock(size_t size) {
    for (auto& block : blocks) {
        if (!block.used && block.size >= size) {
            return &block;
        }
    }
    return nullptr;
}

void MemoryAllocator::splitBlock(MemoryBlock* block, size_t size) {
    if (block->size > size + sizeof(MemoryBlock)) {
        MemoryBlock newBlock;
        newBlock.memory = static_cast<char*>(block->memory) + size;
        newBlock.size = block->size - size;
        newBlock.used = false;

        block->size = size;

        // Find the block in the vector
        auto it = std::find_if(blocks.begin(), blocks.end(),
            [block](const MemoryBlock& b) { return &b == block; });
        if (it != blocks.end()) {
            blocks.insert(it + 1, newBlock);
        }
    }
}

void MemoryAllocator::mergeFreeBlocks() {
    for (size_t i = 0; i < blocks.size() - 1; ) {
        MemoryBlock& current = blocks[i];
        MemoryBlock& next = blocks[i + 1];

        if (!current.used && !next.used &&
            static_cast<char*>(current.memory) + current.size == next.memory) {
            // Merge the blocks
            current.size += next.size;
            blocks.erase(blocks.begin() + i + 1);
        } else {
            i++;
        }
    }
}