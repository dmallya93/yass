# AdaChess - Chess Engine

A modern C++ chess engine migrated from Ada, focusing on maintainability and performance.

## Overview

AdaChess is a chess engine that provides move generation, position evaluation, and game play capabilities. This C++ version preserves the safety and correctness principles of the original Ada implementation while leveraging modern C++ features and tooling.

## Requirements

- **C++ Compiler**: Clang 20+ or GCC 14+
- **CMake**: 3.20 or higher
- **Conan**: 2.x package manager
- **Build Tools**: Make or Ninja
- **Optional**: clang-tidy-20, clang-format-20 for development

## Building from Source

### 1. Install Dependencies

First, ensure Conan is installed and configured:

```bash
# Install Conan (if not already installed)
pip install conan

# Detect and create default profile
conan profile detect --force
```

### 2. Install Project Dependencies

```bash
cd /path/to/adachess
conan install . --build=missing -s build_type=Debug
```

### 3. Configure and Build

Using CMake presets (recommended):

```bash
# Configure for Clang Debug (with sanitizers and clang-tidy)
cmake --preset clang-debug

# Build
cmake --build --preset clang-debug --parallel

# Run the application
./build/clang-debug/adachess
```

Alternative configurations:

```bash
# Clang Release
cmake --preset clang-release
cmake --build --preset clang-release --parallel

# GCC Debug
cmake --preset gcc-debug
cmake --build --preset gcc-debug --parallel

# GCC Release
cmake --preset gcc-release
cmake --build --preset gcc-release --parallel
```

### 4. Run Tests

```bash
# Run all tests
ctest --preset clang-debug

# Run tests with verbose output
ctest --preset clang-debug --verbose
```

## Development

### Code Formatting

Format all source files:

```bash
cmake --build --preset clang-debug --target format
```

Check formatting without modifying files:

```bash
cmake --build --preset clang-debug --target format-check
```

### Static Analysis

Run clang-tidy manually:

```bash
cmake --build --preset clang-debug --target run-clang-tidy
```

Note: clang-tidy runs automatically during builds when `ENABLE_CLANG_TIDY=ON` (default for presets).

### Sanitizers

Debug builds include AddressSanitizer and UndefinedBehaviorSanitizer by default. To disable:

```bash
cmake --preset clang-debug -DENABLE_SANITIZERS=OFF
cmake --build --preset clang-debug
```

### Clean Build

```bash
# Clean build artifacts
cmake --build --preset clang-debug --target clean

# Complete clean (removes CMake cache)
rm -rf build/
```

## Project Structure

```
adachess/
├── include/adachess/      # Public header files
│   └── version.hpp
├── src/                   # Source files
│   └── main.cpp
├── tests/                 # Test files
│   ├── test_main.cpp
│   └── test_version.cpp
├── CMakeLists.txt         # Build configuration
├── CMakePresets.json      # Build presets
├── conanfile.py          # Dependency specification
├── .clang-tidy           # Static analysis config
├── .clang-format         # Code formatting config
└── README.md             # This file
```

## CMake Options

- `CMAKE_BUILD_TYPE`: Build type (Debug, Release, RelWithDebInfo, MinSizeRel)
- `ENABLE_SANITIZERS`: Enable AddressSanitizer and UndefinedBehaviorSanitizer (default: ON for Debug)
- `ENABLE_CLANG_TIDY`: Enable clang-tidy static analysis (default: ON)

## Troubleshooting

### Conan Issues

If you encounter Conan-related errors:

```bash
# Remove Conan cache and reinstall
conan remove "*" --confirm
conan install . --build=missing -s build_type=Debug
```

### Compiler Not Found

Ensure the compiler is in your PATH:

```bash
# Check Clang
which clang++

# Check GCC
which g++
```

### clang-tidy Warnings

If clang-tidy produces warnings that fail the build, you can temporarily disable it:

```bash
cmake --preset clang-debug -DENABLE_CLANG_TIDY=OFF
cmake --build --preset clang-debug
```

## License

This project is licensed under the GNU General Public License v3.0 - see the COPYING file for details.

## Contributing

Contributions are welcome! Please ensure:

1. Code follows the project's formatting standards (run `format` target)
2. All tests pass (`ctest`)
3. No clang-tidy warnings are introduced
4. Sanitizers detect no issues in debug builds
