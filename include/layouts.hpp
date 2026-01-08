#ifndef YASS_LAYOUTS_HPP
#define YASS_LAYOUTS_HPP

#include <filesystem>
#include <string>

namespace layouts {

// Create default site layout (default.html) in the specified directory
// The layout file will be created at {directory_name}/_layouts/default.html
void create_layout(const std::filesystem::path& directory_name);

// Create default directory listing layout (directory.html) for the web server
// The layout file will be created at {directory_name}/_layouts/directory.html
void create_directory_layout(const std::filesystem::path& directory_name);

}  // namespace layouts

#endif  // YASS_LAYOUTS_HPP
