// Copyright (c) 2025 C++ Migration
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

#include "yass/logging.hpp"

#include <gtest/gtest.h>

#include <string>

namespace yass {
namespace logging {
namespace {

// ANSI color codes for verification
constexpr const char* ANSI_RESET = "\033[0m";
constexpr const char* ANSI_RED = "\033[31m";
constexpr const char* ANSI_GREEN = "\033[32m";

// Helper to capture stdout
class StdoutCapture {
 public:
  StdoutCapture() { testing::internal::CaptureStdout(); }

  std::string GetOutput() {
    return testing::internal::GetCapturedStdout();
  }
};

// Test ERROR messages have red color codes
TEST(LoggingTest, ErrorMessageHasRedColor) {
  StdoutCapture capture;
  show_message("Error test", MessageType::ERROR);
  std::string output = capture.GetOutput();

  // Verify red color code is present
  EXPECT_NE(output.find(ANSI_RED), std::string::npos)
      << "Output should contain red color code (\\033[31m)";

  // Verify reset code is present
  EXPECT_NE(output.find(ANSI_RESET), std::string::npos)
      << "Output should contain reset code (\\033[0m)";

  // Verify message text is present
  EXPECT_NE(output.find("Error test"), std::string::npos)
      << "Output should contain the message text";

  // Verify newline is present
  EXPECT_NE(output.find('\n'), std::string::npos)
      << "Output should end with newline";
}

// Test SUCCESS messages have green color codes
TEST(LoggingTest, SuccessMessageHasGreenColor) {
  StdoutCapture capture;
  show_message("Success test", MessageType::SUCCESS);
  std::string output = capture.GetOutput();

  // Verify green color code is present
  EXPECT_NE(output.find(ANSI_GREEN), std::string::npos)
      << "Output should contain green color code (\\033[32m)";

  // Verify reset code is present
  EXPECT_NE(output.find(ANSI_RESET), std::string::npos)
      << "Output should contain reset code (\\033[0m)";

  // Verify message text is present
  EXPECT_NE(output.find("Success test"), std::string::npos)
      << "Output should contain the message text";

  // Verify newline is present
  EXPECT_NE(output.find('\n'), std::string::npos)
      << "Output should end with newline";
}

// Test NORMAL messages have NO color codes
TEST(LoggingTest, NormalMessageHasNoColor) {
  StdoutCapture capture;
  show_message("Normal test", MessageType::NORMAL);
  std::string output = capture.GetOutput();

  // Verify NO color codes are present
  EXPECT_EQ(output.find(ANSI_RED), std::string::npos)
      << "NORMAL message should not contain red color code";

  EXPECT_EQ(output.find(ANSI_GREEN), std::string::npos)
      << "NORMAL message should not contain green color code";

  EXPECT_EQ(output.find(ANSI_RESET), std::string::npos)
      << "NORMAL message should not contain reset code";

  // Verify message text is present
  EXPECT_NE(output.find("Normal test"), std::string::npos)
      << "Output should contain the message text";

  // Verify newline is present
  EXPECT_NE(output.find('\n'), std::string::npos)
      << "Output should end with newline";
}

// Test default message type (should be ERROR with red color)
TEST(LoggingTest, DefaultMessageTypeIsError) {
  StdoutCapture capture;
  show_message("Default type test");
  std::string output = capture.GetOutput();

  // Default type should be ERROR, so red color should be present
  EXPECT_NE(output.find(ANSI_RED), std::string::npos)
      << "Default message type should use red color (ERROR)";
}

// Test that the default message type constant is ERROR
TEST(LoggingTest, DefaultConstant) {
  EXPECT_EQ(DEFAULT_MESSAGE_TYPE, MessageType::ERROR);
}

// Test output format matches Ada version (message + newline, no timestamps)
TEST(LoggingTest, OutputFormatMatchesAda) {
  StdoutCapture capture;
  show_message("Format test", MessageType::NORMAL);
  std::string output = capture.GetOutput();

  // Should be just: "Format test\n" with no timestamps or log levels
  EXPECT_EQ(output, "Format test\n")
      << "Output format should be just message + newline, no timestamps";
}

// Test with string literals
TEST(LoggingTest, StringLiterals) {
  StdoutCapture capture;
  show_message("String literal test", MessageType::NORMAL);
  std::string output = capture.GetOutput();

  EXPECT_NE(output.find("String literal test"), std::string::npos);
}

// Test with std::string
TEST(LoggingTest, StdString) {
  std::string msg = "std::string test";
  StdoutCapture capture;
  show_message(msg, MessageType::SUCCESS);
  std::string output = capture.GetOutput();

  EXPECT_NE(output.find("std::string test"), std::string::npos);
  EXPECT_NE(output.find(ANSI_GREEN), std::string::npos);
}

// Test with long messages
TEST(LoggingTest, LongMessage) {
  std::string long_msg(500, 'x');
  StdoutCapture capture;
  show_message(long_msg, MessageType::NORMAL);
  std::string output = capture.GetOutput();

  EXPECT_NE(output.find(long_msg), std::string::npos);
}

// Test with special characters
TEST(LoggingTest, SpecialCharacters) {
  StdoutCapture capture;
  show_message("Message with\ttabs", MessageType::NORMAL);
  std::string output = capture.GetOutput();

  EXPECT_NE(output.find("Message with\ttabs"), std::string::npos);
}

// Test with Unicode characters
TEST(LoggingTest, UnicodeCharacters) {
  StdoutCapture capture;
  show_message("Unicode: 你好世界 🌍", MessageType::SUCCESS);
  std::string output = capture.GetOutput();

  EXPECT_NE(output.find("Unicode: 你好世界 🌍"), std::string::npos);
  EXPECT_NE(output.find(ANSI_GREEN), std::string::npos);
}

// Test that color codes are in correct order (color, text, reset)
TEST(LoggingTest, ColorCodeOrder) {
  StdoutCapture capture;
  show_message("Order test", MessageType::ERROR);
  std::string output = capture.GetOutput();

  size_t red_pos = output.find(ANSI_RED);
  size_t text_pos = output.find("Order test");
  size_t reset_pos = output.find(ANSI_RESET);

  // All should be found
  ASSERT_NE(red_pos, std::string::npos);
  ASSERT_NE(text_pos, std::string::npos);
  ASSERT_NE(reset_pos, std::string::npos);

  // Order should be: red < text < reset
  EXPECT_LT(red_pos, text_pos) << "Red color code should come before text";
  EXPECT_LT(text_pos, reset_pos) << "Text should come before reset code";
}

}  // namespace
}  // namespace logging
}  // namespace yass
