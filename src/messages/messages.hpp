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

#ifndef YASS_MESSAGES_HPP
#define YASS_MESSAGES_HPP

#include <string>

namespace messages {

// Types of messages: NORMAL, ERROR or SUCCESS
enum class message_type { NORMAL, ERROR, SUCCESS };

// Show selected message to the user
// text - Text to show to the user
// msg_type - Type of message. Default is ERROR
void show_message(const std::string& text,
                  message_type msg_type = message_type::ERROR);

} // namespace messages

#endif // YASS_MESSAGES_HPP
