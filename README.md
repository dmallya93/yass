# YASS (Yet Another Static Site Generator) - C++ Migration

This repository contains the C++ migration of YASS, a static site generator originally written in Ada.

Source: https://github.com/dmallya93/yass.git
Created: Wed Jan  7 20:05:06 UTC 2026

## Overview

YASS is a static site generator that converts Markdown files to HTML using configurable layouts. This C++ version aims to preserve all functionality while leveraging modern C++ features and practices.

## Current Status - Milestone 1, Task 5

### Completed Components

- **Messages Module**: User feedback and logging system
  - `show_message()`: Displays messages with color-coded terminal output
  - Supports ERROR (red), SUCCESS (green), and NORMAL message types
  - Used for error reporting throughout the application

- **Layouts Module**: Generates default HTML layout templates for new sites
  - `create_layout()`: Creates default.html with HTML5 structure and template tags
  - `create_directory_layout()`: Creates directory.html for server directory listings
  - Preserves AWS template syntax (@@IF@@, @@TABLE@@) from Ada implementation
  - Layouts match original Ada semantics for future template processing

## Building

### Prerequisites

- CMake 3.20+
- C++20 compatible compiler (GCC 11+ or Clang 12+)
- Google Test (GTest) library

### Build Instructions

```bash
# Configure
cmake --preset default

# Build
cmake --build --preset default --parallel

# Run tests
ctest --preset default
```

## Project Structure

```
├── include/           # Public headers
│   ├── messages.hpp   # Messages module interface
│   └── layouts.hpp    # Layouts module interface
├── src/
│   ├── messages/      # Messages module implementation
│   │   └── messages.cpp
│   └── layouts/       # Layouts module implementation
│       └── layouts.cpp
├── tests/             # Unit tests
│   └── layouts_test.cpp
├── CMakeLists.txt     # Build configuration
├── CMakePresets.json  # CMake presets for different configurations
└── conanfile.py       # Dependency management (optional)
```

## Testing

The project uses Google Test for unit testing. All tests can be run with:

```bash
ctest --preset default --verbose
```

## License

YASS is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
