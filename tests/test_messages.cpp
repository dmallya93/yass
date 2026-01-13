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

#include "messages/messages.hpp"
#include <gtest/gtest.h>
#include <sstream>

// Testing output is challenging, so we'll do basic smoke tests
// In a real-world scenario, you might want to redirect cout for testing

class MessagesTest : public ::testing::Test {
private:
  std::stringstream buffer;
  std::streambuf* old_cout{nullptr};

protected:
  void SetUp() override {
    // Redirect cout to our buffer
    old_cout = std::cout.rdbuf();
    std::cout.rdbuf(buffer.rdbuf());
  }

  void TearDown() override {
    // Restore cout
    std::cout.rdbuf(old_cout);
  }

  std::string get_output() { return buffer.str(); }
};

TEST_F(MessagesTest, ShowMessageNormal) {
  messages::show_message("Test message", messages::message_type::NORMAL);

  std::string output = get_output();
  EXPECT_NE(output.find("Test message"), std::string::npos);
  // Normal messages should not have color codes
  EXPECT_EQ(output.find("\033[31m"), std::string::npos); // No red
  EXPECT_EQ(output.find("\033[32m"), std::string::npos); // No green
}

TEST_F(MessagesTest, ShowMessageError) {
  messages::show_message("Error message", messages::message_type::ERROR);

  std::string output = get_output();
  EXPECT_NE(output.find("Error message"), std::string::npos);
  // Error messages should have red color code
  EXPECT_NE(output.find("\033[31m"), std::string::npos);
  EXPECT_NE(output.find("\033[0m"), std::string::npos); // Reset code
}

TEST_F(MessagesTest, ShowMessageSuccess) {
  messages::show_message("Success message", messages::message_type::SUCCESS);

  std::string output = get_output();
  EXPECT_NE(output.find("Success message"), std::string::npos);
  // Success messages should have green color code
  EXPECT_NE(output.find("\033[32m"), std::string::npos);
  EXPECT_NE(output.find("\033[0m"), std::string::npos); // Reset code
}

TEST_F(MessagesTest, ShowMessageDefaultIsError) {
  // Default message type should be ERROR
  messages::show_message("Default message");

  std::string output = get_output();
  EXPECT_NE(output.find("Default message"), std::string::npos);
  // Should have red color code (ERROR)
  EXPECT_NE(output.find("\033[31m"), std::string::npos);
}

TEST_F(MessagesTest, ShowMessageEmptyString) {
  // Should not crash with empty string
  EXPECT_NO_THROW(messages::show_message("", messages::message_type::NORMAL));
}

TEST_F(MessagesTest, ShowMessageLongText) {
  std::string long_text(1000, 'x');
  EXPECT_NO_THROW(
      messages::show_message(long_text, messages::message_type::NORMAL));

  std::string output = get_output();
  EXPECT_NE(output.find(long_text), std::string::npos);
}

TEST_F(MessagesTest, ShowMessageWithNewlines) {
  messages::show_message("Line1\nLine2\nLine3",
                         messages::message_type::NORMAL);

  std::string output = get_output();
  EXPECT_NE(output.find("Line1"), std::string::npos);
  EXPECT_NE(output.find("Line2"), std::string::npos);
  EXPECT_NE(output.find("Line3"), std::string::npos);
}

TEST_F(MessagesTest, ShowMessageWithSpecialCharacters) {
  messages::show_message("Test with special chars: !@#$%^&*()",
                         messages::message_type::NORMAL);

  std::string output = get_output();
  EXPECT_NE(output.find("!@#$%^&*()"), std::string::npos);
}

TEST_F(MessagesTest, MultipleMessages) {
  messages::show_message("Message 1", messages::message_type::NORMAL);
  messages::show_message("Message 2", messages::message_type::ERROR);
  messages::show_message("Message 3", messages::message_type::SUCCESS);

  std::string output = get_output();
  EXPECT_NE(output.find("Message 1"), std::string::npos);
  EXPECT_NE(output.find("Message 2"), std::string::npos);
  EXPECT_NE(output.find("Message 3"), std::string::npos);
}

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
