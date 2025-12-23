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

#include <iostream>

namespace yass {

void show_message(std::string_view text, MessageType message_type) {
    // ANSI escape code for terminal colors
    constexpr char ESC = '\x1B';

    // Change text color in terminal if needed
    switch (message_type) {
    case MessageType::ERROR:
        std::cout << ESC << "[31m";  // Red color
        break;
    case MessageType::SUCCESS:
        std::cout << ESC << "[32m";  // Green color
        break;
    case MessageType::NORMAL:
        // No color change needed
        break;
    }

    // Print the message text
    std::cout << text;

    // Reset text color in terminal if needed
    if (message_type != MessageType::NORMAL) {
        std::cout << ESC << "[0m";  // Reset color
    }

    std::cout << '\n';
}

}  // namespace yass
