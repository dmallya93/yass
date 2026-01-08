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

  // Generate HTML5 layout matching Ada implementation
  // Includes @@IF@@ conditionals for optional meta tags
  // Note: The conditionals are preserved from the Ada version for semantic
  // parity. Full AWS template syntax processing will be implemented in later
  // milestones.
  const std::string layout_content = R"(<!DOCTYPE html>
<html lang="{%Language%}">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<link rel="canonical" href="{%canonicallink%}">
@@IF@@ {%author%} /= ""
<meta name="author" content="{%author%}">
@@END_IF@@
@@IF@@ {%description%} /= ""
<meta name="description" content="{%description%}">
@@END_IF@@
@@IF@@ {%AtomLink%} /= ""
{%AtomLink%}
@@END_IF@@
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

  // Generate directory listing layout matching Ada implementation
  // Uses @@TABLE@@ construct for iterating over directory entries
  // Note: The @@TABLE@@ syntax is preserved from the Ada version for semantic
  // parity. Full template loop processing will be implemented in later
  // milestones.
  const std::string layout_content = R"(<!DOCTYPE html>
<html>
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
</head>
<body>
@@TABLE@@
<a href="{%NAME_V%}">{%NAME_V%}</a><br />
@@END_TABLE@@
</body>
</html>
)";

  write_layout_file(layout_path, layout_content);
}

}  // namespace layouts
