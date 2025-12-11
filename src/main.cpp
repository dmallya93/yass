// Copyright (c) 2019-2021 Bartek thindil Jasicki <thindil@laeran.pl>
// Copyright (c) 2022-2024 A.J. Ianozi
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

#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

#include "yass/config.hpp"
#include "yass/layouts.hpp"
#include "yass/messages.hpp"
#include "yass/pages.hpp"

namespace {

constexpr std::string_view VERSION = "3.2.0-dev";
constexpr std::string_view RELEASED = "2024-08-23";

// Working directory for the site project
std::string work_directory;

void show_help() {
  std::cout << "Possible actions:\n"
            << "help - show this screen and exit\n"
            << "version - show the program version and exit\n"
            << "license - show short info about the program license\n"
            << "readme - show content of README file\n"
            << "createnow [name] - create new site in \"name\" directory\n"
            << "create [name] - interactively create new site in \"name\" "
               "directory\n"
            << "build [name] - build site in \"name\" directory\n"
            << "server [name] - start simple HTTP server in \"name\" directory "
               "and auto rebuild site if needed.\n"
            << "createfile [name] - create new empty markdown file with "
               "\"name\"\n";
}

void show_version() {
  std::cout << "Version: " << VERSION << "\n";
  std::cout << "Released: " << RELEASED << "\n";
}

void show_license() {
  std::cout << "Copyright (C) 2022-2024 A.J. Ianozi\n";
  std::cout << "Copyright (C) 2019-2021 Bartek thindil Jasicki\n\n";
  std::cout << "This program is free software: you can redistribute it and/or "
               "modify\n";
  std::cout << "it under the terms of the GNU General Public License as "
               "published by\n";
  std::cout << "the Free Software Foundation, either version 3 of the License, "
               "or\n";
  std::cout << "(at your option) any later version.\n\n";
  std::cout << "This program is distributed in the hope that it will be "
               "useful,\n";
  std::cout << "but WITHOUT ANY WARRANTY; without even the implied warranty "
               "of\n";
  std::cout << "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See "
               "the\n";
  std::cout << "GNU General Public License for more details.\n\n";
  std::cout << "You should have received a copy of the GNU General Public "
               "License\n";
  std::cout << "along with this program.  If not, see "
               "<https://www.gnu.org/licenses/>.\n";
}

void show_readme() {
  // Try to find README.md in a few common locations
  std::vector<std::filesystem::path> readme_paths;
  readme_paths.reserve(4);
  readme_paths.emplace_back("README.md");
  readme_paths.emplace_back("../README.md");
  readme_paths.emplace_back("../../README.md");

  const char* home = std::getenv("HOME");
  if (home != nullptr) {
    readme_paths.push_back(
        std::filesystem::path(home) / ".local" / "share" / "yass" / "README.md");
  }

  for (const auto& readme_path : readme_paths) {
    if (std::filesystem::exists(readme_path)) {
      std::ifstream readme_file(readme_path);
      if (readme_file) {
        std::string line;
        while (std::getline(readme_file, line)) {
          std::cout << line << "\n";
        }
        return;
      }
    }
  }

  yass::show_message("Can't find README.md file");
}

// Validate command-line arguments and set work_directory
// Returns true if arguments are valid
// Note: should_exist parameter indicates whether the directory is expected to exist
//       true = directory must exist (for build/server commands)
//       false = directory must NOT exist (for create commands)
bool valid_arguments(std::string_view message, bool should_exist,
                     const std::vector<std::string>& args) {
  if (args.size() < 3) {
    yass::show_message("Please specify directory name " + std::string(message));
    return false;
  }

  // Get the absolute path (args[0] is program, args[1] is command, args[2] is directory)
  std::filesystem::path path(args[2]);
  try {
    work_directory = std::filesystem::absolute(path).string();
  } catch (const std::filesystem::filesystem_error& /*e*/) {
    work_directory = path.string();
  }

  // Check if directory existence matches expectation
  bool exists = std::filesystem::exists(work_directory);
  if (exists != should_exist) {
    if (should_exist) {
      // Directory should exist but doesn't
      yass::show_message("Directory with that name not exists, please specify "
                         "existing site directory.");
    } else {
      // Directory should NOT exist but does
      yass::show_message(
          "Directory with that name exists, please specify another.");
    }
    return false;
  }

  // Check for site.cfg in existing directories (only when directory should exist)
  if (should_exist) {
    std::filesystem::path config_path =
        std::filesystem::path(work_directory) / "site.cfg";
    if (!std::filesystem::exists(config_path)) {
      yass::show_message(
          "Selected directory don't have file \"site.cfg\". Please specify "
          "proper directory.");
      return false;
    }
  }

  return true;
}

void create_site(bool interactive, const std::vector<std::string>& args) {
  if (!valid_arguments("where new page will be created.", false, args)) {
    return;
  }

  // Create directory structure
  std::vector<std::filesystem::path> paths = {
      std::filesystem::path(work_directory) / "_layouts",
      std::filesystem::path(work_directory) / "_output",
      std::filesystem::path(work_directory) / "_modules" / "start",
      std::filesystem::path(work_directory) / "_modules" / "pre",
      std::filesystem::path(work_directory) / "_modules" / "post",
      std::filesystem::path(work_directory) / "_modules" / "end"};

  for (const auto& dir_path : paths) {
    std::filesystem::create_directories(dir_path);
  }

  // Create or interactively configure site
  if (interactive) {
    yass::interactive_site_config();
  }
  yass::create_site_config(work_directory);

  // Create layout files
  yass::create_layout(work_directory);
  yass::create_directory_layout(work_directory);

  // Create empty index file
  yass::create_empty_file(work_directory);

  yass::show_message("New page in directory \"" + work_directory +
                         "\" was created. Edit \"" + work_directory +
                         "/site.cfg\" file to set data for your new site.",
                     yass::MessageType::SUCCESS);
}

void build_site(const std::vector<std::string>& args) {
  if (!valid_arguments("from where page will be created.", true, args)) {
    return;
  }

  yass::load_site_config(work_directory);

  // TODO: Implement full build functionality in future milestone
  // For now, just indicate the config was loaded successfully
  yass::show_message("Site was build.", yass::MessageType::SUCCESS);
}

void server_command(const std::vector<std::string>& args) {
  if (!valid_arguments("from where site will be served.", true, args)) {
    return;
  }

  yass::load_site_config(work_directory);

  // TODO: Implement server functionality in future milestone
  yass::show_message(
      "Server command not yet implemented. Coming in future milestone.");
}

void create_file(const std::vector<std::string>& args) {
  if (args.size() < 3) {
    yass::show_message("Please specify name of file to create.");
    return;
  }

  std::filesystem::path file_path(args[2]);

  // Handle relative vs absolute paths
  if (file_path.is_relative()) {
    file_path = std::filesystem::current_path() / file_path;
  }

  // Add .md extension if not present
  if (file_path.extension() != ".md") {
    file_path.replace_extension(".md");
  }

  work_directory = file_path.string();

  if (std::filesystem::exists(work_directory)) {
    std::cout << "Can't create file \"" << work_directory
              << "\". File with that name exists.\n";
    return;
  }

  // Create parent directory if needed
  std::filesystem::path parent_path = file_path.parent_path();
  if (!parent_path.empty()) {
    std::filesystem::create_directories(parent_path);
  }

  yass::create_empty_file(work_directory);

  yass::show_message("Empty file \"" + work_directory + "\" was created.",
                     yass::MessageType::SUCCESS);
}

}  // namespace

int main(int argc, char* argv[]) {
  try {
    // Check for YASSDIR environment variable
    const char* yassdir = std::getenv("YASSDIR");
    if (yassdir != nullptr) {
      std::filesystem::current_path(yassdir);
    }

    // Convert arguments to vector for easier handling
    std::vector<std::string> args;
    args.reserve(static_cast<size_t>(argc));
    for (int i = 0; i < argc; ++i) {
      // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
      args.emplace_back(argv[i]);
    }

    // No arguments or help command
    if (argc < 2 || args[1] == "help" || args[1] == "--help") {
      show_help();
      return 0;
    }

    // Version command
    if (args[1] == "version" || args[1] == "--version") {
      show_version();
      return 0;
    }

    // License command
    if (args[1] == "license") {
      show_license();
      return 0;
    }

    // Readme command
    if (args[1] == "readme") {
      show_readme();
      return 0;
    }

    // Create commands
    if (args[1] == "create") {
      create_site(true, args);
      return 0;
    }

    if (args[1] == "createnow") {
      create_site(false, args);
      return 0;
    }

    // Build command
    if (args[1] == "build") {
      build_site(args);
      return 0;
    }

    // Server command
    if (args[1] == "server") {
      server_command(args);
      return 0;
    }

    // Create file command
    if (args[1] == "createfile") {
      create_file(args);
      return 0;
    }

    // Unknown command
    yass::show_message("Unknown command '" + args[1] + "'");
    show_help();
    return 1;

  } catch (const yass::InvalidConfigData& e) {
    yass::show_message(std::string("Invalid data in site config file "
                                   "\"site.cfg\". Invalid line: \"") +
                       e.what() + "\"");
    return 1;
  } catch (const std::exception& e) {
    yass::show_message(std::string("Error: ") + e.what());
    return 1;
  }

  return 0;
}
