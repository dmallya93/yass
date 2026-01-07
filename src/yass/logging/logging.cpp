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

#include "yass/logging.hpp"

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

#include <cassert>
#include <memory>

namespace yass::logging {

namespace {

// ANSI color codes matching Ada implementation
constexpr const char* ANSI_RESET = "\033[0m";
constexpr const char* ANSI_RED = "\033[31m";
constexpr const char* ANSI_GREEN = "\033[32m";

// Get or create the YASS logger instance
std::shared_ptr<spdlog::logger> get_logger() {
  static std::shared_ptr<spdlog::logger> logger;
  if (!logger) {
    // Create a stdout color sink (thread-safe)
    auto sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();

    // Create logger with the sink
    logger = std::make_shared<spdlog::logger>("yass", sink);

    // Set pattern to just the message (no timestamps, levels, etc.)
    // This matches Ada's simple Put(Text); New_Line; output
    logger->set_pattern("%v");

    // Set level to trace to allow all messages through
    logger->set_level(spdlog::level::trace);
  }
  return logger;
}

}  // namespace

void show_message(std::string_view text, MessageType message_type) {
  // Precondition: text must not be empty (matching Ada's Pre => Text'Length >
  // 0)
  assert(!text.empty() && "Message text must not be empty");

  auto logger = get_logger();

  // Apply color based on message type
  // Matches Ada implementation: case Message_Type is...
  switch (message_type) {
  case MessageType::ERROR:
    // Put (Item => ESC & "[31m"); Put(Text); Put (Item => ESC & "[0m");
    logger->info("{}{}{}", ANSI_RED, text, ANSI_RESET);
    break;
  case MessageType::SUCCESS:
    // Put (Item => ESC & "[32m"); Put(Text); Put (Item => ESC & "[0m");
    logger->info("{}{}{}", ANSI_GREEN, text, ANSI_RESET);
    break;
  case MessageType::NORMAL:
    // null; Put(Text); (no color codes)
    logger->info("{}", text);
    break;
  }

  // Explicitly flush to ensure message is displayed immediately
  // Matches Ada's implicit flush behavior
  logger->flush();
}

}  // namespace yass::logging
