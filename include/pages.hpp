#ifndef YASS_PAGES_HPP
#define YASS_PAGES_HPP

#include <filesystem>
#include <string>

namespace pages {

// Create an empty markdown file with default tags and comments
// If file_name ends with .md, creates that file directly
// Otherwise, creates index.md in a directory with that name
void create_empty_file(const std::filesystem::path& file_name);

}  // namespace pages

#endif  // YASS_PAGES_HPP
