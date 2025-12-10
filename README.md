# YASS - Yet Another Static Site Generator

C++ implementation of YASS (Yet Another Static Site generator), migrated from Ada.

Source: https://github.com/dmallya93/yass.git
Created: Wed Dec 10 01:38:06 UTC 2025

## About

YASS is a static site generator that converts Markdown files into HTML pages. This is a C++ implementation of the original Ada codebase, preserving all functionality while leveraging modern C++ features.

## Requirements

- C++ compiler with C++20 support (GCC 11+ or Clang 14+)
- CMake 3.20+
- Git (for fetching dependencies)

## Building

```bash
# Configure
cmake --preset gcc-debug

# Build
cmake --build --preset gcc-debug --parallel

# Run tests
ctest --preset gcc-debug --output-on-failure
```

## Project Structure

- `include/yass/` - Public header files
- `src/` - Implementation files
- `tests/` - Unit tests using Google Test
- `build/` - Build artifacts (gitignored)

## License

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.
