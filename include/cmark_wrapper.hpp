#ifndef YASS_CMARK_WRAPPER_HPP
#define YASS_CMARK_WRAPPER_HPP

#include <string>

namespace cmark {

// Convert markdown text to HTML using libcmark
// text: The markdown text to convert
// html_enabled: If true, allows raw HTML in markdown; if false, sanitizes it
// Returns the converted HTML string
std::string markdown_to_html(const std::string& text, bool html_enabled);

}  // namespace cmark

#endif  // YASS_CMARK_WRAPPER_HPP
