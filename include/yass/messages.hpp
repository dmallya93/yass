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

#ifndef YASS_MESSAGES_HPP
#define YASS_MESSAGES_HPP

#include <cstdint>
#include <string>
#include <string_view>

namespace yass {

/// Types of messages that can be displayed to the user
enum class MessageType : std::uint8_t {
  NORMAL,   ///< Normal message (default terminal color)
  ERROR,    ///< Error message (red color)
  SUCCESS   ///< Success message (green color)
};

/// Default message type for the show_message function
constexpr MessageType DEFAULT_MESSAGE_TYPE = MessageType::ERROR;

/// Show a message to the user with optional color coding
///
/// Displays a message to the console with ANSI color codes based on the message type.
/// Error messages are displayed in red, success messages in green, and normal messages
/// use the default terminal color. The message is always followed by a newline.
///
/// @param text The message text to display (must not be empty)
/// @param message_type The type of message (default: ERROR)
///
/// @throws std::invalid_argument if text is empty
void show_message(std::string_view text,
                  MessageType message_type = DEFAULT_MESSAGE_TYPE);

}  // namespace yass

#endif  // YASS_MESSAGES_HPP
