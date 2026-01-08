#include "messages.hpp"

#include <iostream>

namespace messages {

void show_message(std::string_view text, MessageType type) {
  // ANSI escape codes for terminal colors
  const char* color_code = nullptr;
  const char* reset_code = "\033[0m";

  switch (type) {
    case MessageType::ERROR:
      color_code = "\033[31m";  // Red
      break;
    case MessageType::SUCCESS:
      color_code = "\033[32m";  // Green
      break;
    case MessageType::NORMAL:
      color_code = nullptr;  // No color
      break;
  }

  // Print with color if needed
  if (color_code != nullptr) {
    std::cout << color_code;
  }

  std::cout << text;

  if (color_code != nullptr) {
    std::cout << reset_code;
  }

  std::cout << '\n';
}

}  // namespace messages
