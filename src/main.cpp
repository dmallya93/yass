#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

#include "config.hpp"
#include "layouts.hpp"
#include "messages.hpp"
#include "pages.hpp"

// Version information
constexpr const char* VERSION = "0.1.0";
constexpr const char* RELEASED = "2024-01-08";

// Global work directory
std::filesystem::path work_directory;

// Show help information
void show_help() {
  std::cout << "Yet Another Static Site (generator) (C++ version)\n\n";
  std::cout << "Available commands:\n";
  std::cout << "  help, --help      Show this help message\n";
  std::cout << "  version, --version Show version information\n";
  std::cout << "  license           Show license information\n";
  std::cout << "  readme            Show project readme\n";
  std::cout << "  create [dir]      Create new site with interactive "
               "configuration\n";
  std::cout << "  createnow [dir]   Create new site with default configuration\n";
  std::cout << "  build [dir]       Build existing site (not yet implemented)\n";
  std::cout << "  server [dir]      Start development server (not yet "
               "implemented)\n";
  std::cout << "  createfile [name] Create new page file (not yet implemented)\n";
}

// Show version information
void show_version() {
  std::cout << "Version: " << VERSION << '\n';
  std::cout << "Released: " << RELEASED << '\n';
}

// Show license information
void show_license() {
  std::cout << "Copyright (C) 2022-2024 A.J. Ianozi\n";
  std::cout << "Copyright (C) 2019-2021 Bartek thindil Jasicki\n";
  std::cout << '\n';
  std::cout << "This program is free software: you can redistribute it and/or "
               "modify\n";
  std::cout << "it under the terms of the GNU General Public License as "
               "published by\n";
  std::cout << "the Free Software Foundation, either version 3 of the License, "
               "or\n";
  std::cout << "(at your option) any later version.\n";
  std::cout << '\n';
  std::cout << "This program is distributed in the hope that it will be useful,\n";
  std::cout << "but WITHOUT ANY WARRANTY; without even the implied warranty of\n";
  std::cout << "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n";
  std::cout << "GNU General Public License for more details.\n";
  std::cout << '\n';
  std::cout << "You should have received a copy of the GNU General Public License\n";
  std::cout << "along with this program.  If not, see "
               "<https://www.gnu.org/licenses/>.\n";
}

// Show readme information
void show_readme() {
  std::cout << "YASS - Yet Another Static Site (generator)\n";
  std::cout << '\n';
  std::cout << "A simple static site generator written in C++.\n";
  std::cout << '\n';
  std::cout << "For full documentation, please visit:\n";
  std::cout << "https://github.com/thindil/yass\n";
}

// Validate command-line arguments
// Returns true if valid, false otherwise
bool valid_arguments(const std::vector<std::string>& args,
                     const std::string& message, bool must_exist) {
  // Check if directory name was provided
  // args[0] = program name, args[1] = command, args[2] = directory
  if (args.size() < 3) {
    messages::show_message("Please specify directory name " + message,
                           messages::MessageType::ERROR);
    return false;
  }

  // Get and convert to absolute path
  std::filesystem::path path(args[2]);
  try {
    // Convert to absolute path - if path doesn't exist yet, we need to handle it
    if (std::filesystem::exists(path)) {
      work_directory = std::filesystem::absolute(path);
    } else {
      // For non-existent paths, convert relative to absolute manually
      work_directory = std::filesystem::absolute(path);
    }
  } catch (const std::filesystem::filesystem_error& e) {
    messages::show_message("Invalid path: " + std::string(e.what()),
                           messages::MessageType::ERROR);
    return false;
  }

  // Check existence requirements
  bool exists = std::filesystem::exists(work_directory);
  if (exists == must_exist) {
    if (must_exist) {
      messages::show_message(
          "Directory with that name exists, please specify another.",
          messages::MessageType::ERROR);
    } else {
      messages::show_message("Directory with that name not exists, please "
                             "specify existing site directory.",
                             messages::MessageType::ERROR);
    }
    return false;
  }

  // For existing directories, verify it's a valid YASS site
  if (!must_exist) {
    std::filesystem::path config_file = work_directory / "site.cfg";
    if (!std::filesystem::exists(config_file)) {
      messages::show_message(
          "Selected directory doesn't have file \"site.cfg\". Please specify "
          "proper directory.",
          messages::MessageType::ERROR);
      return false;
    }
  }

  return true;
}

// Create new site with interactive or default configuration
void create_site(const std::vector<std::string>& args, bool interactive) {
  // Validate arguments - directory must NOT exist
  if (!valid_arguments(args, "where new page will be created.", true)) {
    return;
  }

  try {
    // Create directory structure
    std::vector<std::filesystem::path> directories = {
        work_directory / "_layouts",
        work_directory / "_output",
        work_directory / "_modules" / "start",
        work_directory / "_modules" / "pre",
        work_directory / "_modules" / "post",
        work_directory / "_modules" / "end"};

    for (const auto& dir : directories) {
      std::filesystem::create_directories(dir);
    }

    // Get site configuration
    if (interactive) {
      config::interactive_site_config();
    }

    // Create site configuration file
    config::create_site_config(work_directory);

    // Create default layouts
    layouts::create_layout(work_directory);
    layouts::create_directory_layout(work_directory);

    // Create initial index.md file
    pages::create_empty_file(work_directory);

    // Show success message
    std::string success_msg =
        "New page in directory \"" + work_directory.string() +
        "\" was created. Edit \"" + work_directory.string() +
        "/site.cfg\" file to set data for your new site.";
    messages::show_message(success_msg, messages::MessageType::SUCCESS);

  } catch (const std::exception& e) {
    messages::show_message("Error creating site: " + std::string(e.what()),
                           messages::MessageType::ERROR);
  }
}

int main(int argc, char* argv[]) {
  // Convert arguments to vector for easier handling
  std::vector<std::string> args;
  for (int i = 0; i < argc; ++i) {
    args.emplace_back(argv[i]);
  }

  // Check for YASSDIR environment variable
  if (const char* yassdir = std::getenv("YASSDIR")) {
    try {
      std::filesystem::current_path(yassdir);
    } catch (const std::filesystem::filesystem_error& e) {
      messages::show_message(
          "Warning: Failed to change to YASSDIR: " + std::string(e.what()),
          messages::MessageType::ERROR);
    }
  }

  // No arguments or help
  if (argc < 2 || args[1] == "help" || args[1] == "--help") {
    show_help();
    return 0;
  }

  // Version
  if (args[1] == "version" || args[1] == "--version") {
    show_version();
    return 0;
  }

  // License
  if (args[1] == "license") {
    show_license();
    return 0;
  }

  // Readme
  if (args[1] == "readme") {
    show_readme();
    return 0;
  }

  // Create with interactive configuration
  if (args[1] == "create") {
    create_site(args, true);
    return 0;
  }

  // Create with default configuration
  if (args[1] == "createnow") {
    create_site(args, false);
    return 0;
  }

  // Build (not yet implemented)
  if (args[1] == "build") {
    messages::show_message("Build command not yet implemented",
                           messages::MessageType::ERROR);
    return 1;
  }

  // Server (not yet implemented)
  if (args[1] == "server") {
    messages::show_message("Server command not yet implemented",
                           messages::MessageType::ERROR);
    return 1;
  }

  // Create file (not yet implemented)
  if (args[1] == "createfile") {
    messages::show_message("Createfile command not yet implemented",
                           messages::MessageType::ERROR);
    return 1;
  }

  // Unknown command
  messages::show_message("Unknown command: " + args[1],
                         messages::MessageType::ERROR);
  std::cout << "\nRun 'yass help' for list of available commands.\n";
  return 1;
}
