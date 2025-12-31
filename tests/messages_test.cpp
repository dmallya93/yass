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

#include "yass/messages.hpp"

#include <gtest/gtest.h>

#include <sstream>

namespace yass {
namespace {

// Test fixture for Messages tests
class MessagesTest : public ::testing::Test {
private:
    // Capture stdout to verify output
    std::stringstream captured_output;
    std::streambuf* old_stdout{nullptr};

protected:
    void SetUp() override {
        // Redirect stdout to our stringstream
        old_stdout = std::cout.rdbuf(captured_output.rdbuf());
    }

    void TearDown() override {
        // Restore stdout
        std::cout.rdbuf(old_stdout);
    }

    std::string get_output() {
        return captured_output.str();
    }

    void reset_output() {
        captured_output.str("");
        captured_output.clear();
    }
};

// Test showing an error message (default behavior)
TEST_F(MessagesTest, ShowErrorMessage) {
    show_message("Test error message.");
    std::string output = get_output();

    // Should contain the message text
    EXPECT_TRUE(output.find("Test error message.") != std::string::npos);

    // Should contain red color code (ESC[31m)
    EXPECT_TRUE(output.find("\x1B[31m") != std::string::npos);

    // Should contain reset code (ESC[0m)
    EXPECT_TRUE(output.find("\x1B[0m") != std::string::npos);

    // Should end with newline
    EXPECT_TRUE(output.back() == '\n');
}

// Test showing a normal message
TEST_F(MessagesTest, ShowNormalMessage) {
    show_message("Test normal message.", MessageType::NORMAL);
    std::string output = get_output();

    // Should contain the message text
    EXPECT_TRUE(output.find("Test normal message.") != std::string::npos);

    // Should NOT contain color codes for NORMAL messages
    EXPECT_TRUE(output.find("\x1B[31m") == std::string::npos);
    EXPECT_TRUE(output.find("\x1B[32m") == std::string::npos);
    EXPECT_TRUE(output.find("\x1B[0m") == std::string::npos);

    // Should end with newline
    EXPECT_TRUE(output.back() == '\n');
}

// Test showing a success message
TEST_F(MessagesTest, ShowSuccessMessage) {
    show_message("Test success message.", MessageType::SUCCESS);
    std::string output = get_output();

    // Should contain the message text
    EXPECT_TRUE(output.find("Test success message.") != std::string::npos);

    // Should contain green color code (ESC[32m)
    EXPECT_TRUE(output.find("\x1B[32m") != std::string::npos);

    // Should contain reset code (ESC[0m)
    EXPECT_TRUE(output.find("\x1B[0m") != std::string::npos);

    // Should end with newline
    EXPECT_TRUE(output.back() == '\n');
}

// Test that we can call show_message multiple times
TEST_F(MessagesTest, ShowMultipleMessages) {
    show_message("Test error message.");
    reset_output();

    show_message("Test normal message.", MessageType::NORMAL);
    reset_output();

    show_message("Test success message.", MessageType::SUCCESS);
    std::string output = get_output();

    // Last message should be in output
    EXPECT_TRUE(output.find("Test success message.") != std::string::npos);

    // This test mainly verifies that multiple calls don't crash
    SUCCEED();
}

}  // namespace
}  // namespace yass
