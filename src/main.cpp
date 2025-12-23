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

#include "yass/config.hpp"
#include "yass/layouts.hpp"
#include "yass/messages.hpp"
#include "yass/pages.hpp"
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

namespace {

constexpr const char* VERSION = "3.2.0-dev";
constexpr const char* RELEASED = "2024-08-23";

std::string work_directory;

void show_help() {
    std::cout << "Possible actions:\n";
    std::cout << "help - show this screen and exit\n";
    std::cout << "version - show the program version and exit\n";
    std::cout << "license - show short info about the program license\n";
    std::cout << "readme - show content of README file\n";
    std::cout << "createnow [name] - create new site in \"name\" directory\n";
    std::cout << "create [name] - interactively create new site in \"name\" directory\n";
    std::cout << "build [name] - build site in \"name\" directory\n";
    std::cout << "server [name] - start simple HTTP server in \"name\" directory and "
              << "auto rebuild site if needed.\n";
    std::cout << "createfile [name] - create new empty markdown file with \"name\"\n";
}

void show_version() {
    std::cout << "Version: " << VERSION << "\n";
    std::cout << "Released: " << RELEASED << "\n";
}

void show_license() {
    std::cout << "Copyright (C) 2022-2024 A.J. Ianozi\n";
    std::cout << "Copyright (C) 2019-2021 Bartek thindil Jasicki\n";
    std::cout << "\n";
    std::cout << "This program is free software: you can redistribute it and/or modify\n";
    std::cout << "it under the terms of the GNU General Public License as published by\n";
    std::cout << "the Free Software Foundation, either version 3 of the License, or\n";
    std::cout << "(at your option) any later version.\n";
    std::cout << "\n";
    std::cout << "This program is distributed in the hope that it will be useful,\n";
    std::cout << "but WITHOUT ANY WARRANTY; without even the implied warranty of\n";
    std::cout << "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n";
    std::cout << "GNU General Public License for more details.\n";
    std::cout << "\n";
    std::cout << "You should have received a copy of the GNU General Public License\n";
    std::cout << "along with this program.  If not, see <https://www.gnu.org/licenses/>.\n";
}

void show_readme() {
    namespace fs = std::filesystem;

    // Try to find README.md in multiple locations
    std::vector<fs::path> readme_paths = {
        "README.md",
        "../README.md",
        "../../README.md",
        fs::path(std::getenv("HOME") ? std::getenv("HOME") : "") / ".local/share/yass/README.md",
        "/usr/local/share/yass/README.md",
        "/usr/share/yass/README.md"
    };

    for (const auto& readme_path : readme_paths) {
        if (fs::exists(readme_path)) {
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

    yass::show_message("Can't find README.md file", yass::MessageType::ERROR);
}

bool valid_arguments(const std::vector<std::string>& args, const std::string& message, bool is_new_site) {
    namespace fs = std::filesystem;

    // User did not enter name of the site project directory
    // args[0] = program name, args[1] = command, args[2] = directory
    if (args.size() < 3) {
        yass::show_message("Please specify directory name " + message, yass::MessageType::ERROR);
        return false;
    }

    // Get the directory path and convert to absolute path
    try {
        work_directory = fs::absolute(args[2]).string();
    } catch (const std::exception& e) {
        yass::show_message("Invalid directory path: " + args[1], yass::MessageType::ERROR);
        return false;
    }

    // Check if selected directory exists
    bool exists = fs::exists(work_directory);

    // For new sites (create/createnow), directory should NOT exist
    // For existing sites (build/server), directory SHOULD exist
    if (exists == is_new_site) {
        if (is_new_site) {
            yass::show_message("Directory with that name exists, please specify another.",
                             yass::MessageType::ERROR);
        } else {
            yass::show_message("Directory with that name not exists, please specify existing site directory.",
                             yass::MessageType::ERROR);
        }
        return false;
    }

    // Check if selected directory is a valid program site project directory (for existing sites)
    if (!is_new_site) {
        fs::path config_file = fs::path(work_directory) / "site.cfg";
        if (!fs::exists(config_file)) {
            yass::show_message("Selected directory don't have file \"site.cfg\". Please specify proper directory.",
                             yass::MessageType::ERROR);
            return false;
        }
    }

    return true;
}

void create_site(bool interactive) {
    namespace fs = std::filesystem;

    const std::string& path = work_directory;

    // Create directory structure
    const std::vector<std::string> PATHS = {
        "_layouts",
        "_output",
        "_modules/start",
        "_modules/pre",
        "_modules/post",
        "_modules/end"
    };

    for (const auto& dir : PATHS) {
        fs::path full_path = fs::path(path) / dir;
        fs::create_directories(full_path);
    }

    // Create configuration
    if (interactive) {
        yass::interactive_site_config();
    }
    yass::create_site_config(work_directory);

    // Create layouts
    yass::create_layout(path);
    yass::create_directory_layout(path);

    // Create empty index file
    yass::create_empty_file(path);

    yass::show_message("New page in directory \"" + path +
                      "\" was created. Edit \"" + path + "/site.cfg" +
                      "\" file to set data for your new site.",
                      yass::MessageType::SUCCESS);
}

void build_site(const std::vector<std::string>& args) {
    if (!valid_arguments(args, "from where page will be created.", false)) {
        return;
    }

    yass::load_site_config(work_directory);

    // TODO: Full implementation in future milestones
    // For now, just show a placeholder message
    yass::show_message("Site was build.", yass::MessageType::SUCCESS);
}

void start_server(const std::vector<std::string>& args) {
    if (!valid_arguments(args, "from where site will be served.", false)) {
        return;
    }

    yass::load_site_config(work_directory);

    // TODO: Full implementation in future milestones
    yass::show_message("Server functionality not yet implemented.", yass::MessageType::ERROR);
}

void create_file(const std::vector<std::string>& args) {
    namespace fs = std::filesystem;

    if (args.size() < 3) {
        yass::show_message("Please specify name of file to create.", yass::MessageType::ERROR);
        return;
    }

    fs::path file_path(args[2]);

    // If the path is relative, make it absolute relative to current directory
    if (file_path.is_relative()) {
        file_path = fs::current_path() / file_path;
    }

    // Add .md extension if not present
    if (file_path.extension() != ".md") {
        file_path.replace_extension(".md");
    }

    work_directory = file_path.string();

    // Check if file already exists
    if (fs::exists(file_path)) {
        yass::show_message("Can't create file \"" + file_path.string() +
                         "\". File with that name exists.",
                         yass::MessageType::ERROR);
        return;
    }

    // Create parent directories if needed
    if (file_path.has_parent_path()) {
        fs::create_directories(file_path.parent_path());
    }

    // Need to load config to get markdown_comment
    // Try to find site.cfg in current or parent directories
    fs::path config_search = fs::current_path();
    fs::path previous_search;
    while (config_search != previous_search) {
        if (fs::exists(config_search / "site.cfg")) {
            yass::load_site_config(config_search.string());
            break;
        }
        previous_search = config_search;
        config_search = config_search.parent_path();
    }

    // If no config found, yass_conf will already have defaults initialized
    // We don't need to do anything special - just use the default markdown_comment

    yass::create_empty_file(file_path.string());

    yass::show_message("Empty file \"" + file_path.string() + "\" was created.",
                      yass::MessageType::SUCCESS);
}

}  // anonymous namespace

int main(int argc, char* argv[]) {
    try {
        // Check for YASSDIR environment variable
        const char* yassdir = std::getenv("YASSDIR");
        if (yassdir != nullptr) {
            std::filesystem::current_path(yassdir);
        }

        // Convert arguments to vector
        std::vector<std::string> args;
        args.reserve(static_cast<size_t>(argc));
        for (int i = 0; i < argc; ++i) {
            args.emplace_back(argv[i]);
        }

        // No arguments or help: show available commands
        if (argc < 2 || args[1] == "help" || args[1] == "--help") {
            show_help();
            return 0;
        }

        // Show version information
        if (args[1] == "version" || args[1] == "--version") {
            show_version();
            return 0;
        }

        // Show license information
        if (args[1] == "license") {
            show_license();
            return 0;
        }

        // Show README.md file
        if (args[1] == "readme") {
            show_readme();
            return 0;
        }

        // Create new site project directory
        if (args[1] == "create" || args[1] == "createnow") {
            if (!valid_arguments(args, "where new page will be created.", true)) {
                return 1;
            }
            create_site(args[1] == "create");
            return 0;
        }

        // Build site
        if (args[1] == "build") {
            build_site(args);
            return 0;
        }

        // Start server
        if (args[1] == "server") {
            start_server(args);
            return 0;
        }

        // Create new empty markdown file
        if (args[1] == "createfile") {
            create_file(args);
            return 0;
        }

        // Unknown command
        yass::show_message("Unknown command '" + args[1] + "'", yass::MessageType::ERROR);
        show_help();
        return 1;

    } catch (const yass::InvalidConfigData& e) {
        yass::show_message("Invalid data in site config file \"site.cfg\". Invalid line:\"" +
                         std::string(e.what()) + "\"",
                         yass::MessageType::ERROR);
        return 1;
    } catch (const std::exception& e) {
        yass::show_message("Error: " + std::string(e.what()), yass::MessageType::ERROR);
        return 1;
    }

    return 0;
}
