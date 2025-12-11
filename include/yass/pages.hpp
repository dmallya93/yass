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

#ifndef YASS_PAGES_HPP
#define YASS_PAGES_HPP

#include <string>
#include <string_view>

namespace yass {

/// Create an empty markdown file with template comments and tags
///
/// Creates a new markdown file at the specified path with helpful comments
/// explaining the YASS template system and tag usage. If the file_name has
/// a .md extension, the file is created at that path. Otherwise, an index.md
/// file is created in the directory specified by file_name.
///
/// @param file_name Path to the file or directory where the markdown file will be created
/// @throws std::runtime_error if unable to create the file
void create_empty_file(std::string_view file_name);

}  // namespace yass

#endif  // YASS_PAGES_HPP
