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

#include "markdown/cmark.hpp"
#include <iostream>
#include <string>
#include <cstring>

// Simple test harness without external dependencies
namespace {
    int tests_passed = 0;
    int tests_failed = 0;

    void assert_true(bool condition, const char* message) {
        if (condition) {
            ++tests_passed;
            std::cout << "[PASS] " << message << "\n";
        } else {
            ++tests_failed;
            std::cout << "[FAIL] " << message << "\n";
        }
    }

    void assert_contains(const std::string& haystack, const std::string& needle, const char* message) {
        if (haystack.find(needle) != std::string::npos) {
            ++tests_passed;
            std::cout << "[PASS] " << message << "\n";
        } else {
            ++tests_failed;
            std::cout << "[FAIL] " << message << " - '" << needle << "' not found in output\n";
        }
    }

    void assert_not_contains(const std::string& haystack, const std::string& needle, const char* message) {
        if (haystack.find(needle) == std::string::npos) {
            ++tests_passed;
            std::cout << "[PASS] " << message << "\n";
        } else {
            ++tests_failed;
            std::cout << "[FAIL] " << message << " - '" << needle << "' should not be in output\n";
        }
    }
}

int main() {
    std::cout << "Testing CMark wrapper...\n\n";

    // Test 1: Basic heading conversion
    {
        std::string markdown = "# Hello World";
        std::string html = cmark::markdown_to_html(markdown, false);
        assert_contains(html, "<h1>", "Test 1: Heading should contain <h1>");
        assert_contains(html, "Hello World", "Test 1: Heading should contain text");
        assert_contains(html, "</h1>", "Test 1: Heading should be closed");
    }

    // Test 2: Paragraph conversion
    {
        std::string markdown = "This is a paragraph.";
        std::string html = cmark::markdown_to_html(markdown, false);
        assert_contains(html, "<p>", "Test 2: Paragraph should contain <p>");
        assert_contains(html, "This is a paragraph.", "Test 2: Paragraph should contain text");
        assert_contains(html, "</p>", "Test 2: Paragraph should be closed");
    }

    // Test 3: HTML removal when html_enabled is false
    {
        std::string markdown = "<script>alert('xss')</script>";
        std::string html = cmark::markdown_to_html(markdown, false);
        assert_not_contains(html, "<script>", "Test 3: HTML should be removed when html_enabled is false");
        // cmark omits HTML with a comment when UNSAFE option is not set
        assert_contains(html, "<!-- raw HTML omitted -->", "Test 3: HTML should be replaced with omission comment");
    }

    // Test 4: HTML passthrough when html_enabled is true
    {
        std::string markdown = "<div class='custom'>Content</div>";
        std::string html = cmark::markdown_to_html(markdown, true);
        assert_contains(html, "<div class='custom'>", "Test 4: HTML should pass through when html_enabled is true");
        assert_contains(html, "</div>", "Test 4: HTML closing tag should pass through");
    }

    // Test 5: Multiple markdown elements
    {
        std::string markdown = "# Title\n\nParagraph with **bold** text.\n\n- List item 1\n- List item 2";
        std::string html = cmark::markdown_to_html(markdown, false);
        assert_contains(html, "<h1>", "Test 5: Should contain heading");
        assert_contains(html, "<p>", "Test 5: Should contain paragraph");
        assert_contains(html, "<strong>", "Test 5: Should contain bold");
        assert_contains(html, "<ul>", "Test 5: Should contain list");
        assert_contains(html, "<li>", "Test 5: Should contain list items");
    }

    // Test 6: Empty string
    {
        std::string markdown;
        std::string html = cmark::markdown_to_html(markdown, false);
        assert_true(html.empty() || html == "\n", "Test 6: Empty markdown should produce empty or minimal output");
    }

    // Test 7: Code block
    {
        std::string markdown = "```\ncode here\n```";
        std::string html = cmark::markdown_to_html(markdown, false);
        assert_contains(html, "<code>", "Test 7: Code block should contain <code>");
        assert_contains(html, "code here", "Test 7: Code block should contain text");
    }

    // Summary
    std::cout << "\n========================================\n";
    std::cout << "Tests passed: " << tests_passed << "\n";
    std::cout << "Tests failed: " << tests_failed << "\n";
    std::cout << "========================================\n";

    return tests_failed == 0 ? 0 : 1;
}
