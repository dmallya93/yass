# CMark Wrapper Module

This module provides a C++ wrapper around the libcmark library for converting CommonMark markdown to HTML.

## Overview

The module wraps the `cmark_markdown_to_html` C function to provide a convenient C++ interface with proper memory management and exception safety.

## Interface

### Function: `markdown_to_html`

```cpp
std::string markdown_to_html(std::string_view text, bool html_enabled);
```

**Parameters:**
- `text`: The markdown text to convert (accepts `std::string_view` to avoid unnecessary copies)
- `html_enabled`: If `true`, allows raw HTML to pass through unchanged (sets `CMARK_OPT_UNSAFE` flag). If `false`, raw HTML is omitted with a comment.

**Returns:** 
- The HTML output as a `std::string`

**Exception Safety:**
- Strong exception guarantee
- Throws `std::runtime_error` if cmark returns null (should not happen in normal use)
- Uses RAII (`std::unique_ptr` with custom deleter) to ensure proper memory cleanup

## Example Usage

```cpp
#include "markdown/cmark.hpp"
#include <iostream>

int main() {
    // Basic markdown conversion
    std::string markdown = "# Hello World\n\nThis is **bold** text.";
    std::string html = cmark::markdown_to_html(markdown, false);
    std::cout << html << "\n";
    
    // With HTML passthrough enabled
    std::string with_html = "# Title\n\n<div class='custom'>Content</div>";
    std::string html2 = cmark::markdown_to_html(with_html, true);
    std::cout << html2 << "\n";
    
    return 0;
}
```

## Dependencies

- libcmark (or libcmark-gfm) - CommonMark C library
- C++20 standard library

## Implementation Details

### Memory Management

The wrapper properly manages C string memory:
1. Input: Converts `std::string_view` to C string using `.data()` and `.length()`
2. Processing: Calls `cmark_markdown_to_html` which allocates a new C string
3. Cleanup: Uses `std::unique_ptr` with `free` as custom deleter to ensure memory is freed
4. Output: Converts C string to `std::string` before cleanup

### HTML Safety

When `html_enabled` is `false`, cmark removes raw HTML and replaces it with `<!-- raw HTML omitted -->` comments. This is safer than escaping as it prevents any HTML injection attacks.

When `html_enabled` is `true`, the `CMARK_OPT_UNSAFE` flag (0x20000) is set, allowing HTML to pass through unchanged.

## Building

The module is built as a static library `libmarkdown.a` and linked into applications that need markdown conversion.

```bash
# Configure
cmake --preset default

# Build
cmake --build --preset default --parallel

# Test
ctest --preset default
```

## Testing

See `tests/test_cmark.cpp` for comprehensive test cases covering:
- Basic markdown elements (headings, paragraphs, lists, code blocks)
- HTML removal when html_enabled is false
- HTML passthrough when html_enabled is true
- Edge cases (empty input, complex documents)
