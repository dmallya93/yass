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
#include <iostream>

namespace messages {

void show_message(const std::string& text, message_type msg_type) {
  // Change text color in terminal if needed
  switch (msg_type) {
  case message_type::ERROR:
    std::cout << "\033[31m"; // Red
    break;
  case message_type::SUCCESS:
    std::cout << "\033[32m"; // Green
    break;
  case message_type::NORMAL:
    // No color change
    break;
  }

  std::cout << text;

  // Reset text color in terminal if needed
  if (msg_type != message_type::NORMAL) {
    std::cout << "\033[0m"; // Reset
  }

  std::cout << std::endl;
}

} // namespace messages
