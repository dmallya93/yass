//    Copyright 2019-2021 Bartek thindil Jasicki & 2025 J. Quorning
//
//    This file is part of YASS.
//
//    YASS is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    YASS is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with YASS.  If not, see <http://www.gnu.org/licenses/>.

#ifndef YASS_CMARK_HPP
#define YASS_CMARK_HPP

#include <string>
#include <string_view>

namespace cmark {

/// Convert markdown text to HTML using the cmark library.
///
/// This function wraps the cmark_markdown_to_html C function, providing
/// a convenient C++ interface for markdown-to-HTML conversion.
///
/// @param text The markdown text to convert
/// @param html_enabled If true, allow raw HTML passthrough (sets CMARK_OPT_UNSAFE)
/// @return The HTML output as a string
std::string markdown_to_html(std::string_view text, bool html_enabled);

} // namespace cmark

#endif // YASS_CMARK_HPP
