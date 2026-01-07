//    Copyright 2019-2021 Bartek thindil Jasicki & 2022-2024 A.J. Ianozi
//
//    This file is part of YASS.
//
//    YASS is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    YASS is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with YASS.  If not, see <http://www.gnu.org/licenses/>.

#include <CLI/CLI.hpp>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

#include "yass/config.hpp"
#include "yass/logging.hpp"

namespace fs = std::filesystem;

namespace {
constexpr const char* VERSION = "3.2.0-dev";
constexpr const char* RELEASED = "2024-08-23";

// GPL License text
constexpr const char* LICENSE_TEXT = R"(Copyright (C) 2022-2024 A.J. Ianozi
Copyright (C) 2019-2021 Bartek thindil Jasicki

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.)";

// README content (embedded)
constexpr const char* README_TEXT = R"(YASS: Yet Another Static Site (Generator)

YASS is a static site generator written in Ada. It is *headless*
application (no user interface). The program documentation is included in
distribution.

## Features

* Support almost infinite amount of custom tags in HTML templates
* Separated tags for whole site and each page
* Fast
* Can be extended with modules written in any script/programming language
* Generating sitemaps
* Generating Atom feeds
* Auto reconfigure server when configuration file was changed

## Downloading YASS

The easiest way to get YASS is by downloading the latest release for your operating system.

## Build from sources

### Using alire

The easiest way to build YASS is via Alire (https://alire.ada.dev/).  If you don't
 have alire already, you can get it from the main website or auto-install it via
 Getada.dev (https://www.getada.dev/).

To build and install it, simply run `alr install yass` and yass will automatically be
added to alire's binary folder (default is `$HOME/.alire/bin`).

This should automatically pull all of the dependencies in, ie AWS and libcmark.)";

// Helper function to validate site directory
bool validate_site_directory(const std::string& dir, bool should_exist) {
  fs::path path{dir};

  // Convert to absolute path
  if (path.is_relative()) {
    path = fs::absolute(path);
  }

  // Check existence
  if (fs::exists(path) != should_exist) {
    if (should_exist) {
      yass::logging::show_message(
          "Directory with that name not exists, please specify existing site "
          "directory.",
          yass::logging::MessageType::ERROR);
    } else {
      yass::logging::show_message(
          "Directory with that name exists, please specify another.",
          yass::logging::MessageType::ERROR);
    }
    return false;
  }

  // If directory should exist, check for site.cfg
  if (should_exist && !fs::exists(path / "site.cfg")) {
    yass::logging::show_message(
        "Selected directory don't have file \"site.cfg\". Please specify "
        "proper directory.",
        yass::logging::MessageType::ERROR);
    return false;
  }

  return true;
}

// Helper function to create directory structure for new site
void create_directory_structure(const fs::path& site_path) {
  const std::vector<std::string> directories = {
      "_layouts", "_output", "_modules/start", "_modules/pre",
      "_modules/post", "_modules/end"};

  for (const auto& dir : directories) {
    fs::create_directories(site_path / dir);
  }
}

// Helper function to ask user for input with default value
std::string ask_user(const std::string& prompt, const std::string& default_value) {
  std::cout << prompt << "\n";
  std::cout << "(Default - " << default_value << ") > ";
  std::string answer;
  std::getline(std::cin, answer);
  if (answer.empty()) {
    return default_value;
  }
  return answer;
}

// Helper function to ask yes/no question
bool ask_yes_no(const std::string& prompt, bool default_value) {
  std::string default_str = default_value ? "yes" : "no";
  std::string answer = ask_user(prompt, default_str);
  if (answer.empty()) {
    return default_value;
  }
  return answer == "yes" || answer == "y";
}

// Interactive configuration wizard
void interactive_site_config(yass::config::SiteConfig& config) {
  std::cout << "Now we ask you some questions about your new site.\n";
  std::cout << "You can always change it later by modifying the site configuration\n";
  std::cout << "file. If you just press Enter as a answer, default value will be\n";
  std::cout << "used.\n\n";

  config.site_name = ask_user("Please enter the name of the new site", "New Site");
  std::cout << "\n";

  std::cout << "Please enter the description of the new site.\n";
  std::cout << "It is used to create meta tag for the website (which is showed\n";
  std::cout << "in search engines results) but only if pages don't set own.\n";
  std::cout << "Must be set in one line, no new line allowed.\n";
  config.description = ask_user("", "My new site");
  std::cout << "\n";

  config.language = ask_user("Please enter language code in which the new site will be written", "en");
  std::cout << "\n";

  config.author_name = ask_user("Please enter the author of the new site", "John Doe");
  std::cout << "\n";

  config.author_email = ask_user("Please enter the contact email for the new site", "johndoe@example.com");
  std::cout << "\n";

  config.base_url = ask_user("Please enter base URL of the new site", "http://localhost:8888");
  std::cout << "\n";

  std::cout << "Do you want to create Atom feed for the new site?\n";
  std::cout << "If yes, you must specify source for the feed: tags - create Atom\n";
  std::cout << "entries from proper tags in Markdown files, filename - the path\n";
  std::cout << "(related to the project directory path) to markdown file which\n";
  std::cout << "will be used as a source of atom feed (must have proper tags set\n";
  std::cout << "inside). If you press Enter, creating Atom feed will be disabled\n";
  config.atom_feed_source = ask_user("", "none");
  std::cout << "\n";

  if (config.atom_feed_source != "none") {
    std::string amount_str = ask_user(
        "How much maximum entries should be in the Atom feed?\n"
        "Recommended values are between 10 and 50.",
        "25");
    config.atom_feed_amount = std::stoi(amount_str);
    std::cout << "\n";
  }

  config.sitemap_enabled = ask_yes_no("Do you want to create sitemap file for the new site?", true);
  std::cout << "\n";

  config.html_enabled = ask_yes_no("Do you want to HTML embedded in your markdown to be converted?", true);
  std::cout << "\n";

  bool technical_options = ask_yes_no(
      "Do you want to set more technical options (like configuring\n"
      "build-in web server)?",
      false);
  std::cout << "\n";

  if (technical_options) {
    config.server_enabled = ask_yes_no(
        "Should the program start web server when monitoring for\n"
        "changes in the site?",
        true);
    std::cout << "\n";

    std::string port_str = ask_user(
        "On which port should the web server listening?\n"
        "Possible values are from 1 to 65535. Ports below 1025 require\n"
        "root privileges.",
        "8888");
    config.server_port = std::stoi(port_str);
    std::cout << "\n";

    config.stop_server_on_error = ask_yes_no(
        "Should whole monitoring option stop if encounter any error\n"
        "during the site creation?",
        true);
    std::cout << "\n";

    config.browser_command = ask_user(
        "Full path to the web browser which will be started when the\n"
        "program starts in server mode.",
        "none");
    std::cout << "\n";

    std::string monitor_str = ask_user(
        "How often, in seconds, the program should check for changes\n"
        "in the site files?",
        "5.0");
    config.monitor_interval = std::chrono::duration<double>(std::stod(monitor_str));
    std::cout << "\n";

    std::string config_monitor_str = ask_user(
        "How often, in seconds, the program should check for changes\n"
        "in the site configuration file?",
        "60.0");
    config.monitor_config_interval = std::chrono::duration<double>(std::stod(config_monitor_str));
    std::cout << "\n";
  }

  bool compatibility_options = ask_yes_no(
      "Do you want to set options related to compatibility with other\n"
      "static sites generators?",
      false);
  std::cout << "\n";

  if (compatibility_options) {
    config.start_tag_separator = ask_user("What mark should be used as a start for template tag?", "{%");
    std::cout << "\n";

    config.end_tag_separator = ask_user("What mark should be used as an end for template tag?", "%}");
    std::cout << "\n";

    config.markdown_comment = ask_user(
        "What mark should be used as a start for the comment line in\n"
        "Markdown files?",
        "--");
  }
}

// Command handlers
int cmd_help() {
  std::cout << "Possible actions:\n";
  std::cout << "help - show this screen and exit\n";
  std::cout << "version - show the program version and exit\n";
  std::cout << "license - show short info about the program license\n";
  std::cout << "readme - show content of README file\n";
  std::cout << "createnow [name] - create new site in \"name\" directory\n";
  std::cout << "create [name] - interactively create new site in \"name\" directory\n";
  std::cout << "build [name] - build site in \"name\" directory\n";
  std::cout << "server [name] - start simple HTTP server in \"name\" directory and "
               "auto rebuild site if needed.\n";
  std::cout << "createfile [name] - create new empty markdown file with \"name\"\n";
  return 0;
}

int cmd_version() {
  std::cout << "Version: " << VERSION << "\n";
  std::cout << "Released: " << RELEASED << "\n";
  return 0;
}

int cmd_license() {
  std::cout << LICENSE_TEXT << "\n";
  return 0;
}

int cmd_readme() {
  // Try to load from file first (for installed version)
  const char* yassdir = std::getenv("YASSDIR");
  if (yassdir != nullptr) {
    fs::path readme_path = fs::path(yassdir) / "share" / "yass" / "README.md";
    if (fs::exists(readme_path)) {
      std::ifstream readme_file(readme_path);
      if (readme_file.is_open()) {
        std::cout << readme_file.rdbuf();
        return 0;
      }
    }
  }

  // Fall back to embedded README
  std::cout << README_TEXT << "\n";
  return 0;
}

int cmd_create(const std::string& directory, bool interactive) {
  // Validate directory doesn't exist
  if (!validate_site_directory(directory, false)) {
    return 1;
  }

  fs::path site_path = fs::absolute(directory);

  // Create directory structure
  create_directory_structure(site_path);

  // Create configuration
  yass::config::SiteConfig config;
  if (interactive) {
    interactive_site_config(config);
  }
  // Otherwise use defaults (already set in SiteConfig constructor)

  // Create site.cfg file
  yass::config::create_site_config(site_path.string(), config);

  // TODO: Create default layout and directory layout (requires Layouts module)
  // TODO: Create empty index markdown file (requires Pages module)

  yass::logging::show_message(
      "New page in directory \"" + site_path.string() +
          "\" was created. Edit \"" + site_path.string() +
          "/site.cfg\" file to set data for your new site.",
      yass::logging::MessageType::SUCCESS);

  return 0;
}

int cmd_build(const std::string& directory) {
  // Validate directory exists and has site.cfg
  if (!validate_site_directory(directory, true)) {
    return 1;
  }

  yass::logging::show_message(
      "Build command not yet implemented (requires Pages, Layouts, Modules modules)",
      yass::logging::MessageType::ERROR);
  return 1;
}

int cmd_server(const std::string& directory) {
  // Validate directory exists and has site.cfg
  if (!validate_site_directory(directory, true)) {
    return 1;
  }

  yass::logging::show_message(
      "Server command not yet implemented (requires Server, Monitors modules)",
      yass::logging::MessageType::ERROR);
  return 1;
}

int cmd_createfile([[maybe_unused]] const std::string& filename) {
  yass::logging::show_message(
      "Createfile command not yet implemented (requires Pages module)",
      yass::logging::MessageType::ERROR);
  return 1;
}

}  // namespace

// NOLINTNEXTLINE(bugprone-exception-escape)
int main(int argc, char** argv) {

  // Check for YASSDIR environment variable
  const char* yassdir = std::getenv("YASSDIR");
  if (yassdir != nullptr) {
    try {
      fs::current_path(yassdir);
    } catch (const fs::filesystem_error& e) {
      yass::logging::show_message(
          std::string("Failed to change to YASSDIR: ") + e.what(),
          yass::logging::MessageType::ERROR);
    }
  }

  CLI::App app{"YASS - Yet Another Static Site Generator"};
  app.require_subcommand(0, 1);  // Allow 0 or 1 subcommand

  // Variable to store exit code from command execution
  int exit_code = 0;

  // help command (also handle no subcommand)
  auto* help_cmd = app.add_subcommand("help", "Show available commands and exit");
  help_cmd->callback([&]() { exit_code = cmd_help(); });

  // version command
  auto* version_cmd = app.add_subcommand("version", "Show program version and exit");
  version_cmd->callback([&]() { exit_code = cmd_version(); });

  // license command
  auto* license_cmd = app.add_subcommand("license", "Show license information");
  license_cmd->callback([&]() { exit_code = cmd_license(); });

  // readme command
  auto* readme_cmd = app.add_subcommand("readme", "Show README content");
  readme_cmd->callback([&]() { exit_code = cmd_readme(); });

  // createnow command
  auto* createnow_cmd = app.add_subcommand("createnow", "Create new site with default settings");
  std::string createnow_dir;
  createnow_cmd->add_option("directory", createnow_dir, "Site directory name")
      ->required();
  createnow_cmd->callback([&]() { exit_code = cmd_create(createnow_dir, false); });

  // create command
  auto* create_cmd = app.add_subcommand("create", "Interactively create new site");
  std::string create_dir;
  create_cmd->add_option("directory", create_dir, "Site directory name")->required();
  create_cmd->callback([&]() { exit_code = cmd_create(create_dir, true); });

  // build command
  auto* build_cmd = app.add_subcommand("build", "Build site");
  std::string build_dir = ".";
  build_cmd->add_option("directory", build_dir, "Site directory (default: current directory)");
  build_cmd->callback([&]() { exit_code = cmd_build(build_dir); });

  // server command
  auto* server_cmd = app.add_subcommand("server", "Start development server");
  std::string server_dir = ".";
  server_cmd->add_option("directory", server_dir, "Site directory (default: current directory)");
  server_cmd->callback([&]() { exit_code = cmd_server(server_dir); });

  // createfile command
  auto* createfile_cmd = app.add_subcommand("createfile", "Create new markdown file");
  std::string createfile_name;
  createfile_cmd->add_option("name", createfile_name, "File name")->required();
  createfile_cmd->callback([&]() { exit_code = cmd_createfile(createfile_name); });

  // Parse command line
  try {
    app.parse(argc, argv);

    // If no subcommand provided, show help
    if (app.get_subcommands().empty()) {
      return cmd_help();
    }
  } catch (const CLI::ParseError& e) {
    return app.exit(e);
  } catch (const yass::config::Invalid_Config_Data& e) {
    yass::logging::show_message(
        std::string("Configuration error: ") + e.what(),
        yass::logging::MessageType::ERROR);
    return 1;
  } catch (const std::exception& e) {
    yass::logging::show_message(
        std::string("Error: ") + e.what(),
        yass::logging::MessageType::ERROR);
    return 1;
  }

  return exit_code;
}
