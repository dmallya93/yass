// Copyright (c) 2019-2021 Bartek thindil Jasicki <thindil@laeran.pl>
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

#ifndef YASS_LOGGING_HPP
#define YASS_LOGGING_HPP

#include <string_view>

namespace yass {
namespace logging {

/// Types of messages: NORMAL, ERROR, or SUCCESS
/// Corresponds to Ada's Messages_Types enumeration
enum class MessageType {
  NORMAL,  ///< Normal informational message (default terminal color)
  ERROR,   ///< Error message (red color)
  SUCCESS  ///< Success message (green color)
};

/// Default message type (ERROR, matching Ada's Default_Message_Type)
constexpr MessageType DEFAULT_MESSAGE_TYPE = MessageType::ERROR;

/// Show a message to the user with optional color formatting
///
/// This function displays the provided text to stdout with ANSI color codes
/// based on the message type:
/// - ERROR: Red text (ANSI code 31)
/// - SUCCESS: Green text (ANSI code 32)
/// - NORMAL: Default terminal color (no ANSI codes)
///
/// The message is followed by a newline character.
///
/// @param text The message text to display (must not be empty)
/// @param message_type The type of message (default: ERROR)
///
/// @pre text must not be empty
void show_message(std::string_view text,
                  MessageType message_type = DEFAULT_MESSAGE_TYPE);

}  // namespace logging
}  // namespace yass

#endif  // YASS_LOGGING_HPP
