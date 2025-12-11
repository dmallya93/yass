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

#ifndef YASS_LAYOUTS_HPP
#define YASS_LAYOUTS_HPP

#include <string>
#include <string_view>

namespace yass {

/// Create default site layout in the specified directory
///
/// Creates a default.html layout file in the _layouts subdirectory with
/// a basic HTML5 template including YASS template tags.
///
/// @param directory_name Full path to the directory where the layout will be created
/// @throws std::runtime_error if unable to create the layout file
void create_layout(std::string_view directory_name);

/// Create default layout for directory listing for web server
///
/// Creates a directory.html layout file in the layouts directory for use by
/// the web server when displaying directory listings.
///
/// @param directory_name Full path to the directory where the layout will be created
///                       (may be empty to use the current layouts directory)
/// @throws std::runtime_error if unable to create the layout file
void create_directory_layout(std::string_view directory_name);

}  // namespace yass

#endif  // YASS_LAYOUTS_HPP
