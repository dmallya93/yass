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

#include "cmark.hpp"
#include <cmark.h>
#include <memory>
#include <stdexcept>

namespace cmark {

std::string markdown_to_html(std::string_view text, bool html_enabled) {
    // Set options based on html_enabled flag
    // CMARK_OPT_UNSAFE (0x20000) allows raw HTML passthrough
    const int options = html_enabled ? 0x20000 : 0;
    
    // Call cmark C API
    // Note: cmark_markdown_to_html takes a const char*, length, and options
    char* c_result = cmark_markdown_to_html(
        text.data(),
        text.length(),
        options
    );
    
    // Handle null return (shouldn't happen in normal use, but be defensive)
    if (c_result == nullptr) {
        throw std::runtime_error("cmark_markdown_to_html returned null");
    }
    
    // Convert C string to C++ string and ensure proper cleanup
    // Using unique_ptr with custom deleter to ensure memory is freed
    std::unique_ptr<char, decltype(&free)> result_ptr(c_result, &free);
    std::string result(result_ptr.get());
    
    return result;
}

} // namespace cmark
