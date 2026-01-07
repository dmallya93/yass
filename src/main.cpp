// Copyright 2019-2021 Bartek thindil Jasicki & 2022-2024 A.J. Ianozi
// Copyright 2025 Contributors to the YASS C++ migration
//
// This file is part of YASS.
//
// YASS is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// YASS is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with YASS.  If not, see <http://www.gnu.org/licenses/>.

#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <string_view>
#include <vector>

namespace fs = std::filesystem;

// Version information - keep in sync with CMakeLists.txt
constexpr std::string_view VERSION = "3.2.0-dev";
constexpr std::string_view RELEASED = "2025-01-07";

// Forward declarations
void show_help();
void show_version();
void show_license();
void show_readme();

// Show help information
void show_help() {
    std::cout << "Possible actions:\n"
              << "help - show this screen and exit\n"
              << "version - show the program version and exit\n"
              << "license - show short info about the program license\n"
              << "readme - show content of README file\n"
              << "createnow [name] - create new site in \"name\" directory\n"
              << "create [name] - interactively create new site in \"name\" directory\n"
              << "build [name] - build site in \"name\" directory\n"
              << "server [name] - start simple HTTP server in \"name\" directory and "
              << "auto rebuild site if needed.\n"
              << "createfile [name] - create new empty markdown file with \"name\"\n";
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
    std::cout << "This program is free software: you can redistribute it and/or modify\n";
    std::cout << "it under the terms of the GNU General Public License as published by\n";
    std::cout << "the Free Software Foundation, either version 3 of the License, or\n";
    std::cout << "(at your option) any later version.\n";
    std::cout << '\n';
    std::cout << "This program is distributed in the hope that it will be useful,\n";
    std::cout << "but WITHOUT ANY WARRANTY; without even the implied warranty of\n";
    std::cout << "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n";
    std::cout << "GNU General Public License for more details.\n";
    std::cout << '\n';
    std::cout << "You should have received a copy of the GNU General Public License\n";
    std::cout << "along with this program.  If not, see <https://www.gnu.org/licenses/>.\n";
}

// Show README file content
void show_readme() {
    // Try to find README.md in several locations
    std::vector<fs::path> search_paths = {
        "README.md",                                      // Current directory
        fs::current_path() / "README.md",                 // Explicit current path
        fs::current_path().parent_path() / "README.md",   // Parent directory
    };

    // Check YASSDIR environment variable
    if (const char* yass_dir = std::getenv("YASSDIR")) {
        search_paths.emplace_back(fs::path(yass_dir) / "README.md");
    }

    // Try to find and read README.md
    for (const auto& readme_path : search_paths) {
        if (fs::exists(readme_path)) {
            std::ifstream readme_file(readme_path);
            if (readme_file.is_open()) {
                std::string line;
                while (std::getline(readme_file, line)) {
                    std::cout << line << '\n';
                }
                return;
            }
        }
    }

    // If we couldn't find it, show an error
    std::cerr << "Error: Can't find README.md file\n";
}

// Command dispatcher
int main(int argc, char* argv[]) {
    // Handle YASSDIR environment variable
    if (const char* yass_dir = std::getenv("YASSDIR")) {
        try {
            fs::current_path(yass_dir);
        } catch (const fs::filesystem_error& e) {
            std::cerr << "Error: Cannot change to YASSDIR: " << e.what() << '\n';
            return 1;
        }
    }

    // No arguments or help command
    if (argc < 2 || std::string_view(argv[1]) == "help" ||
        std::string_view(argv[1]) == "--help") {
        show_help();
        return 0;
    }

    // Get the command
    std::string_view command(argv[1]);

    // Dispatch to appropriate handler
    if (command == "version" || command == "--version") {
        show_version();
    } else if (command == "license") {
        show_license();
    } else if (command == "readme") {
        show_readme();
    } else if (command == "createnow" || command == "create") {
        std::cerr << "Error: Command '" << command
                  << "' is not yet implemented in this version.\n";
        std::cerr << "This functionality will be added in a future task.\n";
        return 1;
    } else if (command == "build") {
        std::cerr << "Error: Command 'build' is not yet implemented in this version.\n";
        std::cerr << "This functionality will be added in a future task.\n";
        return 1;
    } else if (command == "server") {
        std::cerr << "Error: Command 'server' is not yet implemented in this version.\n";
        std::cerr << "This functionality will be added in a future task.\n";
        return 1;
    } else if (command == "createfile") {
        std::cerr << "Error: Command 'createfile' is not yet implemented in this version.\n";
        std::cerr << "This functionality will be added in a future task.\n";
        return 1;
    } else {
        std::cerr << "Error: Unknown command '" << command << "'\n\n";
        show_help();
        return 1;
    }

    return 0;
}
