#ifndef YASS_MESSAGES_HPP
#define YASS_MESSAGES_HPP

#include <string>
#include <string_view>

namespace messages {

// Types of messages that can be displayed
enum class MessageType { NORMAL, ERROR, SUCCESS };

// Show a message to the user with optional colored output
// text: The message text to display
// type: The type of message (controls color in terminal)
void show_message(std::string_view text,
                  MessageType type = MessageType::ERROR);

}  // namespace messages

#endif  // YASS_MESSAGES_HPP
