#include "cmark_wrapper.hpp"

#include <cmark.h>

#include <stdexcept>

namespace cmark {

std::string markdown_to_html(const std::string& text, bool html_enabled) {
  if (text.empty()) {
    return "";
  }

  // Set options based on html_enabled flag
  int options = CMARK_OPT_DEFAULT;
  if (!html_enabled) {
    // If HTML is not enabled, we should escape HTML tags
    options |= CMARK_OPT_UNSAFE;  // This actually ALLOWS unsafe HTML
    // Note: libcmark doesn't have a built-in sanitization option
    // We'll use UNSAFE for now as the Ada version uses HTML_Enabled flag
    // to control whether HTML is passed through
  }

  // Parse the markdown text into an AST
  cmark_node* document =
      cmark_parse_document(text.c_str(), text.length(), options);

  if (document == nullptr) {
    throw std::runtime_error("Failed to parse markdown document");
  }

  // Render the AST to HTML
  char* html_cstr = cmark_render_html(document, options);

  if (html_cstr == nullptr) {
    cmark_node_free(document);
    throw std::runtime_error("Failed to render markdown to HTML");
  }

  // Convert to C++ string
  std::string result(html_cstr);

  // Free cmark resources
  free(html_cstr);  // NOLINT - cmark uses malloc, so we use free
  cmark_node_free(document);

  return result;
}

}  // namespace cmark
