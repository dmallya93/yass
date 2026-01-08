#include "layouts.hpp"

#include <fstream>
#include <string_view>

#include "messages.hpp"

namespace layouts {

namespace {

// Write layout content to file with error handling
void write_layout_file(const std::filesystem::path& file_path,
                       const std::string& content) {
  std::ofstream file(file_path);
  if (!file) {
    messages::show_message("Failed to create layout file: " +
                               file_path.string(),
                           messages::MessageType::ERROR);
    throw std::runtime_error("Failed to create layout file: " +
                             file_path.string());
  }

  file << content;
  if (!file) {
    messages::show_message("Failed to write layout file: " + file_path.string(),
                           messages::MessageType::ERROR);
    throw std::runtime_error("Failed to write layout file: " +
                             file_path.string());
  }
}

}  // namespace

void create_layout(const std::filesystem::path& directory_name) {
  // Create _layouts directory if it doesn't exist
  const auto layouts_dir = directory_name / "_layouts";
  std::filesystem::create_directories(layouts_dir);

  const auto layout_path = layouts_dir / "default.html";

  // Generate HTML5 layout for M1 (simple tag substitution only)
  // M1 uses simple string replacement without conditionals or loops
  // Only includes tags that are guaranteed to have values or are optional
  // AWS template syntax (@@IF@@, @@TABLE@@, etc.) deferred to M2
  const std::string layout_content = R"(<!DOCTYPE html>
<html lang="{%Language%}">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<title>{%Name%}</title>
</head>
<body>
{%Content%}
</body>
</html>
)";

  write_layout_file(layout_path, layout_content);
}

void create_directory_layout(const std::filesystem::path& directory_name) {
  // Create _layouts directory if it doesn't exist
  const auto layouts_dir = directory_name / "_layouts";
  std::filesystem::create_directories(layouts_dir);

  const auto layout_path = layouts_dir / "directory.html";

  // Generate directory listing layout for M1 (simple tag substitution only)
  // M1 uses simple string replacement without loops
  // Directory listing functionality will be implemented in M2 with @@TABLE@@
  // For now, create a placeholder layout that can be manually edited
  const std::string layout_content = R"(<!DOCTYPE html>
<html>
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<title>Directory Listing</title>
</head>
<body>
<h1>Directory Listing</h1>
<p>Directory listing will be implemented in Milestone 2.</p>
</body>
</html>
)";

  write_layout_file(layout_path, layout_content);
}

}  // namespace layouts
