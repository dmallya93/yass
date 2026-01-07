# YASS - Yet Another Static Site Generator (C++ Migration)

This repository contains the C++ migration of YASS (Yet Another Static Site Generator) from Ada.

Source: https://github.com/dmallya93/yass.git

## Overview

YASS is a static site generator that converts Markdown files into HTML websites. This C++ migration preserves all functionality of the original Ada implementation while leveraging modern C++20 features and best practices.

## Migration Status

### Milestone 1: Foundation (In Progress)
- ✅ Build System (CMake, Conan)
- ✅ Logging System (messages module migrated)
- ⏳ Configuration System
- ⏳ Basic CLI Infrastructure

## Building

### Prerequisites
- C++20 compatible compiler (GCC 11+ or Clang 14+)
- CMake 3.20+
- Conan package manager
- Python 3.6+ (for Conan)

### Build Instructions

1. Install Conan (if not already installed):
```bash
pip3 install conan==1.64.1
```

2. Set up Conan profile:
```bash
conan profile new default --detect
conan profile update settings.compiler.libcxx=libstdc++11 default
```

3. Install dependencies:
```bash
mkdir -p build
conan install . -if build --build=missing
```

4. Configure and build:
```bash
cmake --preset debug
cmake --build --preset debug
```

5. Run tests:
```bash
ctest --preset debug --output-on-failure
```

## Project Structure

```
include/yass/       - Public header files
src/yass/          - Implementation files organized by module
  logging/         - Logging and messages system
tests/unit/        - Unit tests
```

## License

GNU General Public License v3.0 or later

Copyright (c) 2019-2021 Bartek thindil Jasicki
Copyright (c) 2025 C++ Migration
