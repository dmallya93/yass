# YASS - Yet Another Static Site Generator

A C++20 implementation of the YASS static site generator, migrated from Ada.

## Overview

YASS is a static site generator that processes markdown files, applies layouts, and generates a complete static website. This repository contains the C++ implementation of YASS, migrated from the original Ada codebase.

Source: https://github.com/dmallya93/yass.git

## Building

### Prerequisites

- CMake 3.20 or higher
- C++20 compatible compiler (GCC 11+ or Clang 14+)
- Google Test library

### Build Commands

```bash
# Configure with GCC debug build (includes sanitizers)
cmake --preset gcc-debug

# Build
cmake --build --preset gcc-debug --parallel

# Run tests
ctest --preset gcc-debug --output-on-failure

# Run with clang-tidy enabled
cmake --preset gcc-debug -DENABLE_CLANG_TIDY=ON
cmake --build --preset gcc-debug --parallel
```

### Available Build Presets

- `gcc-debug` - Debug build with GCC and sanitizers
- `gcc-release` - Release build with GCC
- `clang-debug` - Debug build with Clang and sanitizers (requires Clang)
- `clang-release` - Release build with Clang (requires Clang)

## Project Status

Currently implemented:
- **Messages Module** - Console output with color-coded messages (NORMAL, ERROR, SUCCESS)

## License

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.
