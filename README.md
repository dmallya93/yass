# YASS - C++ Port

This is a C++ port of YASS (Yet Another Static Site generator), originally written in Ada.

Source: https://github.com/dmallya93/yass.git
Created: Wed Jan  7 20:05:06 UTC 2026

## Current Status

**Milestone 1 - Task 4: CMark Wrapper** - ✅ Complete

The CMark wrapper module provides a C++ interface to the libcmark CommonMark parser for converting Markdown to HTML.

### Components Implemented

- **Markdown Module** (`src/markdown/`)
  - `cmark.hpp` - Header with `markdown_to_html` function declaration
  - `cmark.cpp` - Implementation wrapping libcmark C API
  - Comprehensive test suite in `tests/test_cmark.cpp`

### Building

```bash
# Prerequisites
sudo apt-get install libcmark-dev

# Configure and build
cmake --preset default
cmake --build --preset default --parallel

# Run tests
cd build && ctest
```

### Features

- ✅ Convert Markdown to HTML using CommonMark specification
- ✅ Optional HTML passthrough mode (raw HTML in markdown)
- ✅ Safe HTML handling (HTML removed when passthrough disabled)
- ✅ Memory-safe C++ wrapper using RAII
- ✅ Exception-safe with proper error handling
- ✅ Comprehensive test coverage

### Architecture

The CMark wrapper provides a simple, idiomatic C++ interface:

```cpp
#include "markdown/cmark.hpp"

std::string markdown = "# Hello World";
std::string html = cmark::markdown_to_html(markdown, false);
// Returns: "<h1>Hello World</h1>\n"
```

The implementation uses `std::unique_ptr` with a custom deleter to ensure proper memory management when interfacing with the C library.
