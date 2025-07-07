# Memory Allocator

[![Build Status](https://github.com/ali009-eng/memory-allocator/actions/workflows/build.yml/badge.svg)](https://github.com/yourusername/memory-allocator/actions)

A custom memory allocator implementation in C++ with block management and allocation tracking.

## Features
- Block-based memory management
- Allocation tracking with file/line information
- Memory leak detection
- First-fit allocation strategy
- Thread-safe implementation (optional)

## Build Instructions
```bash
mkdir build && cd build
cmake ..
make