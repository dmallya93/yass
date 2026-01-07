YASS: Yet Another Static Site (Generator) - C++ Version
=======================================================

YASS is a static site generator, now migrated to C++20. It is a *headless*
application (no user interface).

**Note:** This is an in-progress migration from the original Ada version to C++.
Not all features are yet implemented.

## Status

Currently implemented:
- Basic CLI infrastructure
- Help, version, license, and readme commands
- CMake build system with multiple presets
- Modern C++20 codebase with safety features

Coming soon:
- Configuration parsing
- Markdown to HTML conversion
- Template processing
- Site building
- HTTP server with auto-rebuild
- Module system
- Sitemap and Atom feed generation

## Building from Source

### Requirements

- C++20-capable compiler (GCC 11+, Clang 13+, or MSVC 2019+)
- CMake 3.20 or later
- (Future) libcmark for markdown parsing
- (Future) Boost libraries for HTTP server

### Build Instructions

#### Quick Start (Default Configuration)

```bash
# Configure
cmake --preset default

# Build
cmake --build --preset default --parallel

# Run
./build/yass help
```

#### Using Specific Compiler Presets

```bash
# GCC Release Build
cmake --preset gcc-release
cmake --build --preset gcc-release --parallel
./build/gcc-release/yass version

# Clang Debug Build (with sanitizers)
cmake --preset clang-debug
cmake --build --preset clang-debug --parallel
./build/clang-debug/yass version
```

#### Available Presets

- `default` - Default debug build
- `gcc-debug` - GCC with debug symbols and sanitizers
- `gcc-release` - GCC optimized release build
- `clang-debug` - Clang with debug symbols and sanitizers
- `clang-release` - Clang optimized release build

### Code Quality Tools

```bash
# Format code
cmake --build --preset default --target format

# Check formatting
cmake --build --preset default --target format-check

# Run clang-tidy
cmake --build --preset default --target run-clang-tidy
```

## Running the Program

### Basic Usage

```bash
# Show help
./build/yass help

# Show version
./build/yass version

# Show license
./build/yass license

# Show README
./build/yass readme
```

### Environment Variables

- `YASSDIR` - Set this to specify the working directory for YASS operations

Example (Linux/macOS): `export YASSDIR=$(pwd)`
Example (Windows): `set YASSDIR=C:\yass`

## Features (Planned)

* Support for custom tags in HTML templates
* Separated tags for whole site and each page
* Fast performance
* Extensible with modules in any language
* Sitemap generation
* Atom feed generation
* Auto-reconfigure server on configuration changes
* Modern C++20 with strong type safety
* Memory-safe with smart pointers and RAII
* Concurrent processing for performance

## Original Project

This is a C++ migration of YASS, originally written in Ada by Bartek thindil Jasicki
and A.J. Ianozi. The original project can be found at:
https://github.com/yet-another-static-site-generator/yass

## Contributing

For detailed information about contributing (bug reporting, ideas, code conduct, etc.),
see CONTRIBUTING.md (to be added).

## License

* YASS is released under GNU GPL v3 license.
* Original YASS (Ada version): Copyright (C) 2022-2024 A.J. Ianozi and 2019-2021 Bartek thindil Jasicki
* C++ Migration: Copyright (C) 2025 Contributors to the YASS C++ migration

## Project Structure

```
yass/
├── CMakeLists.txt          # Main build configuration
├── CMakePresets.json       # Build presets for different configurations
├── conanfile.py            # Conan package dependencies (future)
├── .clang-format           # Code formatting rules
├── .clang-tidy             # Static analysis rules
├── src/
│   └── main.cpp            # Main entry point and command dispatcher
└── README.md               # This file
```

## Development

This project follows modern C++ best practices:
- C++20 standard
- RAII and smart pointers for memory management
- STL containers and algorithms
- Exception-based error handling (Ada compatibility)
- CMake for build system
- clang-format for consistent code style
- clang-tidy for static analysis

---

Bartek thindil Jasicki & A.J. Ianozi (Original Ada Version)
Contributors to the C++ Migration (2025)
