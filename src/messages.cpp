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

#include <iostream>
#include <stdexcept>

namespace yass {

namespace {
// ANSI escape codes for terminal colors
constexpr const char* ANSI_RED = "\033[31m";
constexpr const char* ANSI_GREEN = "\033[32m";
constexpr const char* ANSI_RESET = "\033[0m";
}  // namespace

void show_message(std::string_view text, MessageType message_type) {
  // Validate precondition: text must not be empty
  if (text.empty()) {
    throw std::invalid_argument("Message text cannot be empty");
  }

  // Apply color based on message type
  switch (message_type) {
    case MessageType::ERROR:
      std::cout << ANSI_RED;
      break;
    case MessageType::SUCCESS:
      std::cout << ANSI_GREEN;
      break;
    case MessageType::NORMAL:
      // No color change for normal messages
      break;
  }

  // Display the message
  std::cout << text;

  // Reset color if it was changed
  if (message_type != MessageType::NORMAL) {
    std::cout << ANSI_RESET;
  }

  // Always end with a newline
  std::cout << '\n';
}

}  // namespace yass
