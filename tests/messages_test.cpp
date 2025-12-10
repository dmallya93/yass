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

#include "yass/messages.hpp"

#include <gtest/gtest.h>

#include <iostream>
#include <sstream>
#include <stdexcept>

namespace yass {
namespace {

// Test fixture for capturing console output
class MessagesTest : public ::testing::Test {
 protected:
  void SetUp() override {
    // Redirect std::cout to our stringstream
    old_cout_buffer_ = std::cout.rdbuf();
    std::cout.rdbuf(output_buffer_.rdbuf());
  }

  void TearDown() override {
    // Restore std::cout
    std::cout.rdbuf(old_cout_buffer_);
  }

  std::string get_output() {
    std::string output = output_buffer_.str();
    output_buffer_.str("");  // Clear the buffer
    output_buffer_.clear();
    return output;
  }

 private:
  std::stringstream output_buffer_;
  std::streambuf* old_cout_buffer_{nullptr};
};

// Test showing error message (default type)
TEST_F(MessagesTest, ShowErrorMessage) {
  show_message("Test error message.");
  std::string output = get_output();

  // Verify message contains text
  EXPECT_NE(output.find("Test error message."), std::string::npos);

  // Verify error color code is present (red: ESC[31m)
  EXPECT_NE(output.find("\033[31m"), std::string::npos);

  // Verify reset code is present
  EXPECT_NE(output.find("\033[0m"), std::string::npos);

  // Verify newline at end
  EXPECT_EQ(output.back(), '\n');
}

// Test showing normal message
TEST_F(MessagesTest, ShowNormalMessage) {
  show_message("Test normal message.", MessageType::NORMAL);
  std::string output = get_output();

  // Verify message contains text
  EXPECT_NE(output.find("Test normal message."), std::string::npos);

  // Verify no color codes are present
  EXPECT_EQ(output.find("\033[31m"), std::string::npos);  // No red
  EXPECT_EQ(output.find("\033[32m"), std::string::npos);  // No green
  EXPECT_EQ(output.find("\033[0m"), std::string::npos);   // No reset

  // Verify newline at end
  EXPECT_EQ(output.back(), '\n');
}

// Test showing success message
TEST_F(MessagesTest, ShowSuccessMessage) {
  show_message("Test success message.", MessageType::SUCCESS);
  std::string output = get_output();

  // Verify message contains text
  EXPECT_NE(output.find("Test success message."), std::string::npos);

  // Verify success color code is present (green: ESC[32m)
  EXPECT_NE(output.find("\033[32m"), std::string::npos);

  // Verify reset code is present
  EXPECT_NE(output.find("\033[0m"), std::string::npos);

  // Verify newline at end
  EXPECT_EQ(output.back(), '\n');
}

// Test that empty text throws exception
TEST_F(MessagesTest, EmptyTextThrowsException) {
  EXPECT_THROW(show_message(""), std::invalid_argument);
}

// Test multiple message types in sequence (like the Ada test)
TEST_F(MessagesTest, MultipleMessages) {
  // This test verifies the function can be called multiple times without crash
  // Similar to the Ada test: "This test can only crash."
  EXPECT_NO_THROW({
    show_message("Test error message.");
    show_message("Test normal message.", MessageType::NORMAL);
    show_message("Test success message.", MessageType::SUCCESS);
  });
}

}  // namespace
}  // namespace yass
