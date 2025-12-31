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

## Usage

### Create a new site

```bash
# Create a new site with default settings
./yass createnow my-site

# Create a new site with interactive configuration
./yass create my-site
```

### Build the site

```bash
# Build the site (generates HTML from markdown files)
./yass build my-site
```

### Create markdown files

```bash
# Create an empty markdown file with template comments
./yass createfile my-site/page.md
```

### Additional commands

```bash
# Show help
./yass help

# Show version
./yass version

# Show license
./yass license

# Show README
./yass readme
```

## Project Status

### Milestone 1 - Foundation, CLI, Configuration, and Messages

**Completed:**
- **Messages Module** - Console output with color-coded messages (NORMAL, ERROR, SUCCESS)
- **Configuration Module** - Parse and manage site.cfg files with all configuration settings
- **CLI Entry Point** - Full command-line interface with argument parsing and routing
- **Site Creation** - Create new site projects with directory structure and default files
- **Layout Stubs** - Create default layout templates (default.html, directory.html)
- **Pages Stubs** - Create empty markdown files with template comments

**Commands implemented:**
- `help`, `version`, `license`, `readme` - Information commands
- `create`, `createnow` - Site creation with interactive/default configuration
- `createfile` - Create empty markdown files
- `build` - Skeleton implementation (loads config, placeholder message)
- `server` - Skeleton placeholder (not yet functional)

**Not yet implemented:**
- Full site building (pages generation, modules processing, layouts application)
- Sitemaps and Atom feed generation
- Web server and file monitoring
- Hot-reload functionality

## License

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.
