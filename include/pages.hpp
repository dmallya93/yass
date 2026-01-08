#ifndef YASS_PAGES_HPP
#define YASS_PAGES_HPP

#include <filesystem>
#include <string>
#include <map>
#include <vector>

namespace pages {

// Create an empty markdown file with default tags and comments
// If file_name ends with .md, creates that file directly
// Otherwise, creates index.md in a directory with that name
void create_empty_file(const std::filesystem::path& file_name);

// Process a markdown file and generate HTML output
// file_name: Path to the markdown file to process
// directory: Base directory for relative path calculation
void create_page(const std::filesystem::path& file_name,
                const std::filesystem::path& directory);

// Copy a non-markdown file to the output directory
// file_name: Path to the file to copy
// directory: Base directory for relative path calculation
void copy_file(const std::filesystem::path& file_name,
              const std::filesystem::path& directory);

}  // namespace pages

#endif  // YASS_PAGES_HPP
