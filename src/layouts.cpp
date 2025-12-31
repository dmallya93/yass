// Copyright (c) 2019-2021 Bartek thindil Jasicki <thindil@laeran.pl>
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
#include "yass/config.hpp"
#include <filesystem>
#include <fstream>
#include <stdexcept>

namespace yass {

void create_layout(const std::string& directory_name) {
    namespace fs = std::filesystem;

    fs::path layout_path = fs::path(directory_name) / "_layouts" / "default.html";

    std::ofstream layout_file(layout_path);
    if (!layout_file) {
        throw std::runtime_error("Failed to create layout file: " + layout_path.string());
    }

    layout_file << "<!DOCTYPE html>\n";
    layout_file << "<html lang=\"{%Language%}\">\n";
    layout_file << "<head>\n";
    layout_file << "<meta charset=\"UTF-8\">\n";
    layout_file << "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n";
    layout_file << "<link rel=\"canonical\" href=\"{%canonicallink%}\">\n";
    layout_file << "@@IF@@ {%author%} /= \"\"\n";
    layout_file << "<meta name=\"author\" content=\"{%author%}\">\n";
    layout_file << "@@END_IF@@\n";
    layout_file << "@@IF@@ {%description%} /= \"\"\n";
    layout_file << "<meta name=\"description\" content=\"{%description%}\">\n";
    layout_file << "@@END_IF@@\n";
    layout_file << "@@IF@@ {%AtomLink%} /= \"\"\n";
    layout_file << "{%AtomLink%}\n";
    layout_file << "@@END_IF@@\n";
    layout_file << "<title>{%Name%}</title>\n";
    layout_file << "</head>\n";
    layout_file << "<body>\n";
    layout_file << "{%Content%}\n";
    layout_file << "</body>\n";
    layout_file << "</html>\n";
}

void create_directory_layout(const std::string& directory_name) {
    namespace fs = std::filesystem;

    fs::path layout_path;

    if (directory_name.empty()) {
        // Server mode: use configuration's layout directory
        layout_path = fs::path(yass_conf.layouts_directory) / "directory.html";
    } else {
        // Creation mode: use standard _layouts directory
        layout_path = fs::path(directory_name) / "_layouts" / "directory.html";
    }

    std::ofstream layout_file(layout_path);
    if (!layout_file) {
        throw std::runtime_error("Failed to create directory layout file: " + layout_path.string());
    }

    layout_file << "<!DOCTYPE html>\n";
    layout_file << "<html>\n";
    layout_file << "<head>\n";
    layout_file << "<meta charset=\"UTF-8\">\n";
    layout_file << "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n";
    layout_file << "</head>\n";
    layout_file << "<body>\n";
    layout_file << "@@TABLE@@\n";
    layout_file << "<a href=\"{%NAME_V%}\">{%NAME_V%}</a><br />\n";
    layout_file << "@@END_TABLE@@\n";
    layout_file << "</body>\n";
    layout_file << "</html>\n";
}

}  // namespace yass
