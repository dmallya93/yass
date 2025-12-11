// Copyright (c) 2019-2021 Bartek thindil Jasicki <thindil@laeran.pl>
// Copyright (c) 2025 C++ migration
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include "yass/layouts.hpp"

#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <string>
#include <string_view>

#include "yass/config.hpp"

namespace yass {

void create_layout(std::string_view directory_name) {
  std::filesystem::path layout_path =
      std::filesystem::path(directory_name) / "_layouts" / "default.html";

  std::ofstream layout_file(layout_path);
  if (!layout_file) {
    throw std::runtime_error("Failed to create layout file: " +
                             layout_path.string());
  }

  layout_file << "<!DOCTYPE html>\n"
              << "<html lang=\"{%Language%}\">\n"
              << "<head>\n"
              << "<meta charset=\"UTF-8\">\n"
              << "<meta name=\"viewport\" content=\"width=device-width, "
                 "initial-scale=1.0\">\n"
              << "<link rel=\"canonical\" href=\"{%canonicallink%}\">\n"
              << "@@IF@@ {%author%} /= \"\"\n"
              << "<meta name=\"author\" content=\"{%author%}\">\n"
              << "@@END_IF@@\n"
              << "@@IF@@ {%description%} /= \"\"\n"
              << "<meta name=\"description\" content=\"{%description%}\">\n"
              << "@@END_IF@@\n"
              << "@@IF@@ {%AtomLink%} /= \"\"\n"
              << "{%AtomLink%}\n"
              << "@@END_IF@@\n"
              << "<title>{%Name%}</title>\n"
              << "</head>\n"
              << "<body>\n"
              << "{%Content%}\n"
              << "</body>\n"
              << "</html>\n";

  layout_file.close();
  if (!layout_file) {
    throw std::runtime_error("Failed to write layout file: " +
                             layout_path.string());
  }
}

void create_directory_layout(std::string_view directory_name) {
  std::filesystem::path layout_path;

  if (directory_name.empty()) {
    // Use the global config to get layouts directory
    layout_path = std::filesystem::path(yass_conf.layouts_directory) /
                  "directory.html";
  } else {
    layout_path = std::filesystem::path(directory_name) /
                  yass_conf.layouts_directory / "directory.html";
  }

  std::ofstream layout_file(layout_path);
  if (!layout_file) {
    throw std::runtime_error("Failed to create directory layout file: " +
                             layout_path.string());
  }

  layout_file << "<!DOCTYPE html>\n"
              << "<html>\n"
              << "<head>\n"
              << "<meta charset=\"UTF-8\">\n"
              << "<meta name=\"viewport\" content=\"width=device-width, "
                 "initial-scale=1.0\">\n"
              << "</head>\n"
              << "<body>\n"
              << "@@TABLE@@\n"
              << "<a href=\"{%NAME_V%}\">{%NAME_V%}</a><br />\n"
              << "@@END_TABLE@@\n"
              << "</body>\n"
              << "</html>\n";

  layout_file.close();
  if (!layout_file) {
    throw std::runtime_error("Failed to write directory layout file: " +
                             layout_path.string());
  }
}

}  // namespace yass
