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

#include "yass/config.hpp"

#include <algorithm>
#include <cctype>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string_view>

namespace yass {

// Global state variables
ParserConfig yass_conf;
TagsContainer site_tags;
std::string site_directory;
TableTagsContainer global_table_tags;

namespace {

// Helper function to trim whitespace from both ends of a string
std::string trim(std::string_view str) {
    auto start = str.begin();
    while (start != str.end() && std::isspace(static_cast<unsigned char>(*start))) {
        ++start;
    }

    auto end = str.end();
    do {
        --end;
    } while (std::distance(start, end) > 0 && std::isspace(static_cast<unsigned char>(*end)));

    return std::string(start, end + 1);
}

// Helper function to convert string to lowercase
std::string to_lower(std::string str) {
    std::transform(str.begin(), str.end(), str.begin(), [](unsigned char c) { return std::tolower(c); });
    return str;
}

// Helper function to normalize directory paths (convert relative to absolute)
void normalize_dir(std::string& directory_path, const std::string& base_directory) {
    namespace fs = std::filesystem;

    fs::path dir_path(directory_path);
    if (!dir_path.is_absolute()) {
        dir_path = fs::path(base_directory) / dir_path;
        directory_path = dir_path.string();
    }
}

// Helper function to write a line to config file
void write_config_line(std::ofstream& file, std::string_view line) {
    file << line << '\n';
}

// Helper function to split string by delimiter
std::vector<std::string> split_string(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::stringstream ss(str);
    std::string token;

    while (std::getline(ss, token, delimiter)) {
        tokens.push_back(trim(token));
    }

    return tokens;
}

}  // anonymous namespace

void create_site_config(const std::string& directory_name) {
    namespace fs = std::filesystem;

    fs::path config_path = fs::path(directory_name) / "site.cfg";
    std::ofstream config_file(config_path, std::ios::app);

    if (!config_file) {
        throw std::runtime_error("Failed to create config file: " + config_path.string());
    }

    // Write configuration file with comments
    write_config_line(config_file, "# Directory in which will be placed HTML files with site layout");
    write_config_line(config_file, "# (templates). May be absolute or relative to project directory.");
    write_config_line(config_file, "LayoutsDirectory = _layouts");
    write_config_line(config_file, "");

    write_config_line(config_file, "# Directory in which will be placed generated site. May be absolute");
    write_config_line(config_file, "# or relative to project directory.");
    write_config_line(config_file, "OutputDirectory = _output");
    write_config_line(config_file, "");

    write_config_line(config_file, "# Directory in which will be placed program modules used to generate");
    write_config_line(config_file, "# the site. May be absolute or relative to project directory.");
    write_config_line(config_file, "ModulesDirectory = _modules");
    write_config_line(config_file, "");

    write_config_line(config_file, "# List of excluded files and directories from list of sources used to");
    write_config_line(config_file, "# generating the site. All paths must be relative to the project");
    write_config_line(config_file, "# directory. If you exclude directory, it whole content will be");
    write_config_line(config_file, "# excluded too. Layouts, modules and output directories are excluded");
    write_config_line(config_file, "# by default.");
    write_config_line(config_file, "ExcludedFiles = .git,.gitignore,tags");
    write_config_line(config_file, "");

    write_config_line(config_file, "# The name of the site which will be created. If you have enabled");
    write_config_line(config_file, "# creating Atom feed then it is needed. Otherwise, you can use it as a");
    write_config_line(config_file, "# normal template tag.");
    write_config_line(config_file, "Name = " + yass_conf.site_name);
    write_config_line(config_file, "");

    write_config_line(config_file, "# The description of the site which will be created. Must be in one line,");
    write_config_line(config_file, "# no new line allowed. It is used to set meta tag description (which is");
    write_config_line(config_file, "# showed in search engines results) but only when pages don't set it.");
    write_config_line(config_file, "# Optional setting.");
    write_config_line(config_file, "Description = " + yass_conf.description);
    write_config_line(config_file, "");

    write_config_line(config_file, "# The ISO 639-1 language code in which the site will be created.");
    write_config_line(config_file, "Language = " + yass_conf.language);
    write_config_line(config_file, "");

    write_config_line(config_file, "# Name of author of the site. If you have enable creating Atom feed,");
    write_config_line(config_file, "# then it is needed. Otherwise, you can use it as a normal template tag.");
    write_config_line(config_file, "# It is also used in setting meta tag author for all pages.");
    write_config_line(config_file, "Author = " + yass_conf.author_name);
    write_config_line(config_file, "");

    write_config_line(config_file, "# Email address of author of the site. If you have enable creating Atom");
    write_config_line(config_file, "# feed, then it is needed. Otherwise, you can use it as a normal");
    write_config_line(config_file, "# template tag.");
    write_config_line(config_file, "AuthorEmail = " + yass_conf.author_email);
    write_config_line(config_file, "");

    write_config_line(config_file, "# Base URL of the site. It is needed mostly for creating sitemap and");
    write_config_line(config_file, "# Atom feed, but you can use it as a normal the site tag. If your site");
    write_config_line(config_file, "# will be available at https://mysite.com/blog then this will be your");
    write_config_line(config_file, "# BaseURL.");
    write_config_line(config_file, "BaseURL = " + yass_conf.base_url);
    write_config_line(config_file, "");

    write_config_line(config_file, "# Source which will be used for creating Atom feed of the site.");
    write_config_line(config_file, "# Possible values are: none: don't create atom feed, tags: create");
    write_config_line(config_file, "# Atom entries from proper tags in .md files, [filename]: the path");
    write_config_line(config_file, "# (related to the project directory path) to markdown file which will");
    write_config_line(config_file, "# be used as a source of atom feed (must have proper tags set inside).");
    write_config_line(config_file, "AtomFeedSource = " + yass_conf.atom_feed_source);
    write_config_line(config_file, "");

    write_config_line(config_file, "# Number of entries in the Atom feed of the site. Try not set it too");
    write_config_line(config_file, "# high, recommended values are between 10 and 50.");
    write_config_line(config_file, "AtomFeedAmount = " + std::to_string(yass_conf.atom_feed_amount));
    write_config_line(config_file, "");

    write_config_line(config_file, "# Should the program create sitemap when creating the site. Possible");
    write_config_line(config_file, "# values are true or false (case-insensitive).");
    write_config_line(config_file, "SitemapEnabled = " + std::string(yass_conf.sitemap_enabled ? "true" : "false"));
    write_config_line(config_file, "");

    write_config_line(config_file, "# Should program convert HTML in markdown documents to actual HTML.");
    write_config_line(config_file, "# Possible values are true or false (case-insensitive).");
    write_config_line(config_file, "HTMLEnabled = " + std::string(yass_conf.html_enabled ? "true" : "false"));
    write_config_line(config_file, "");

    write_config_line(config_file, "# Should the program start web server when monitoring for changes in");
    write_config_line(config_file, "# site. Possible values are true or false (case-insensitive).");
    write_config_line(config_file, "ServerEnabled = " + std::string(yass_conf.server_enabled ? "true" : "false"));
    write_config_line(config_file, "");

    write_config_line(config_file, "# Port on which web server will be listen if enabled. Possible values");
    write_config_line(config_file, "# are from 1 to 65535. Please remember, that ports below 1025 require");
    write_config_line(config_file, "# root privileges to work.");
    write_config_line(config_file, "ServerPort = " + std::to_string(yass_conf.server_port));
    write_config_line(config_file, "");

    write_config_line(config_file, "# Should web server and whole monitoring of the site changes stop if");
    write_config_line(config_file, "# encounter any error during the site creation.  Possible values are");
    write_config_line(config_file, "# true or false (case-insensitive).");
    write_config_line(config_file, "StopServerOnError = " + std::string(yass_conf.stop_server_on_error ? "true" : "false"));
    write_config_line(config_file, "");

    write_config_line(config_file, "# Full path to the command which will be used to start the web browser");
    write_config_line(config_file, "# with index.html page of the site. String \"%s\" (without quotes) will");
    write_config_line(config_file, "# be replaced by server URL. If this setting is \"none\", the web");
    write_config_line(config_file, "# browser will be not started, same as when the web server is disabled.");
    write_config_line(config_file, "BrowserCommand = " + yass_conf.browser_command);
    write_config_line(config_file, "");

    write_config_line(config_file, "# How often (in seconds) the program should monitor site for changes");
    write_config_line(config_file, "# and regenerate it if needed. Can be any positive number, but you");
    write_config_line(config_file, "# probably don't want to set it to check every few thousands years :)");
    write_config_line(config_file, "MonitorInterval = " + std::to_string(yass_conf.monitor_interval.count()));
    write_config_line(config_file, "");

    write_config_line(config_file, "# How often (in seconds) the program should monitor site configuration");
    write_config_line(config_file, "# for changes and reconfigure it if needed. Can be any positive number.");
    write_config_line(config_file, "MonitorConfigInterval = " + std::to_string(yass_conf.monitor_config_interval.count()));
    write_config_line(config_file, "");

    write_config_line(config_file, "# String used to mark start of the templates tags, used in templates");
    write_config_line(config_file, "# files. You may want to change it, if you want to use templates from");
    write_config_line(config_file, "# other static site generator.");
    write_config_line(config_file, "StartTagSeparator = " + yass_conf.start_tag_separator);
    write_config_line(config_file, "");

    write_config_line(config_file, "# String used to mark end of the templates tags, used in templates");
    write_config_line(config_file, "# files. You may want to change it, if you want to use templates from");
    write_config_line(config_file, "# other static site generator.");
    write_config_line(config_file, "EndTagSeparator = " + yass_conf.end_tag_separator);
    write_config_line(config_file, "");

    write_config_line(config_file, "# String used to mark comments in markdown files which will be parsed.");
    write_config_line(config_file, "MarkdownComment = " + yass_conf.markdown_comment);
    write_config_line(config_file, "");

    write_config_line(config_file, "# Site tags, optional. Tags can be 4 types: strings, boolean, numeric");
    write_config_line(config_file, "# or composite.");
    write_config_line(config_file, "# First 3 types of tags are in Name = Value scheme. For strings, it can");
    write_config_line(config_file, "# be any alphanumeric value without new line sign. For boolean it must");
    write_config_line(config_file, "# be \"true\" or \"false\", for numeric any number. Program will detect");
    write_config_line(config_file, "# self which type of tag is and properly set it. It always falls back");
    write_config_line(config_file, "# to string value.");
    write_config_line(config_file, "# Composite tags first must be initialized with Name = [] then just");
    write_config_line(config_file, "# add as many as you want values to it by Name = Value scheme.");
    write_config_line(config_file, "# For more information about site.cfg file please check program");
    write_config_line(config_file, "# documentation.");
    write_config_line(config_file, "");

    config_file.close();
}

void load_site_config(const std::string& directory_name) {
    namespace fs = std::filesystem;

    // Clear existing state
    site_tags.clear();
    global_table_tags.clear();

    // Open config file
    fs::path config_path = fs::path(directory_name) / "site.cfg";
    std::ifstream config_file(config_path);

    if (!config_file) {
        throw std::runtime_error("Failed to open config file: " + config_path.string());
    }

    std::string line;
    std::string raw_data;

    while (std::getline(config_file, line)) {
        raw_data = line;

        // Skip empty lines and comments
        if (line.empty() || line[0] == '#') {
            continue;
        }

        // Find the equals sign
        size_t equal_index = line.find('=');
        if (equal_index == std::string::npos) {
            throw InvalidConfigData(raw_data);
        }

        // Extract field name and value (trimming spaces around '=')
        std::string field_name = trim(line.substr(0, equal_index));
        std::string value = trim(line.substr(equal_index + 1));

        try {
            if (field_name == "LayoutsDirectory") {
                yass_conf.layouts_directory = value;

            } else if (field_name == "OutputDirectory") {
                yass_conf.output_directory = value;

            } else if (field_name == "ModulesDirectory") {
                yass_conf.modules_directory = value;

            } else if (field_name == "ExcludedFiles") {
                auto tokens = split_string(value, ',');
                for (const auto& token : tokens) {
                    if (!token.empty()) {
                        yass_conf.excluded_files.push_back(token);
                    }
                }

            } else if (field_name == "ServerEnabled") {
                yass_conf.server_enabled = (to_lower(value) == "true");

            } else if (field_name == "ServerPort") {
                auto port = std::stoul(value);
                if (port > 65535) {
                    throw InvalidConfigData(raw_data);
                }
                yass_conf.server_port = static_cast<std::uint32_t>(port);

            } else if (field_name == "StopServerOnError") {
                yass_conf.stop_server_on_error = (to_lower(value) == "true");

            } else if (field_name == "BrowserCommand") {
                // Replace %s with server URL if present
                size_t pos = value.find("%s");
                if (pos != std::string::npos) {
                    std::string server_url = "http://localhost:" + std::to_string(yass_conf.server_port);
                    value.replace(pos, 2, server_url);
                }
                yass_conf.browser_command = value;

            } else if (field_name == "MonitorInterval") {
                yass_conf.monitor_interval = std::chrono::duration<double>(std::stod(value));

            } else if (field_name == "MonitorConfigInterval") {
                yass_conf.monitor_config_interval = std::chrono::duration<double>(std::stod(value));

            } else if (field_name == "BaseURL") {
                yass_conf.base_url = value;
                site_tags["BaseURL"] = value;

            } else if (field_name == "SitemapEnabled") {
                yass_conf.sitemap_enabled = (to_lower(value) == "true");

            } else if (field_name == "HTMLEnabled") {
                yass_conf.html_enabled = (to_lower(value) == "true");

            } else if (field_name == "AtomFeedSource") {
                if (value == "none" || value == "tags") {
                    yass_conf.atom_feed_source = value;
                } else {
                    // If it's a file path, change extension from .md to .html
                    if (value.size() >= 3 && value.substr(value.size() - 3) == ".md") {
                        yass_conf.atom_feed_source = value.substr(0, value.size() - 3) + ".html";
                    } else {
                        yass_conf.atom_feed_source = value;
                    }
                }

            } else if (field_name == "AtomFeedAmount") {
                yass_conf.atom_feed_amount = static_cast<std::uint32_t>(std::stoul(value));

            } else if (field_name == "Name") {
                yass_conf.site_name = value;
                site_tags[field_name] = value;

            } else if (field_name == "StartTagSeparator") {
                yass_conf.start_tag_separator = value;

            } else if (field_name == "EndTagSeparator") {
                yass_conf.end_tag_separator = value;

            } else if (field_name == "MarkdownComment") {
                yass_conf.markdown_comment = value;

            } else if (field_name == "Author") {
                yass_conf.author_name = value;

            } else if (field_name == "AuthorEmail") {
                yass_conf.author_email = value;

            } else if (field_name == "Language") {
                yass_conf.language = value;
                site_tags[field_name] = value;

            } else if (field_name == "Description") {
                yass_conf.description = value;

            } else if (value == "[]") {
                // Initialize composite tag
                global_table_tags[field_name] = std::vector<std::string>();

            } else if (global_table_tags.contains(field_name)) {
                // Append to existing composite tag
                global_table_tags[field_name].push_back(value);

            } else {
                // Regular site tag
                site_tags[field_name] = value;
            }
        } catch (const InvalidConfigData&) {
            throw;
        } catch (const std::exception&) {
            throw InvalidConfigData(raw_data);
        }
    }

    config_file.close();

    // Normalize directory paths
    normalize_dir(yass_conf.layouts_directory, directory_name);
    normalize_dir(yass_conf.output_directory, directory_name);
    normalize_dir(yass_conf.modules_directory, directory_name);

    // Add default excluded files
    yass_conf.excluded_files.push_back(".");
    yass_conf.excluded_files.push_back("..");
    yass_conf.excluded_files.push_back("site.cfg");

    // Add directory basenames to excluded files
    fs::path layouts_path(yass_conf.layouts_directory);
    fs::path output_path(yass_conf.output_directory);
    fs::path modules_path(yass_conf.modules_directory);

    yass_conf.excluded_files.push_back(layouts_path.filename().string());
    yass_conf.excluded_files.push_back(output_path.filename().string());
    yass_conf.excluded_files.push_back(modules_path.filename().string());

    // Store site directory
    site_directory = directory_name;
}

void interactive_site_config() {
    auto ask_user = [](const std::string& default_value) -> std::string {
        std::cout << "(Default - " << default_value << ") > ";
        std::string answer;
        std::getline(std::cin, answer);
        if (answer.empty()) {
            answer = default_value;
        }
        return answer;
    };

    std::cout << "Now we ask you some questions about your new site.\n";
    std::cout << "You can always change it later by modifying the site configuration\n";
    std::cout << "file. If you just press Enter as a answer, default value will be\n";
    std::cout << "used.\n\n";

    std::cout << "Please enter the name of the new site\n\n";
    yass_conf.site_name = ask_user("New Site");
    std::cout << '\n';

    std::cout << "Please enter the description of the new site.\n";
    std::cout << "It is used to create meta tag for the website (which is showed\n";
    std::cout << "in search engines results) but only if pages don't set own.\n";
    std::cout << "Must be set in one line, no new line allowed.\n\n";
    yass_conf.description = ask_user("My new site");
    std::cout << '\n';

    std::cout << "Please enter language code in which the new site will be written\n\n";
    yass_conf.language = ask_user("en");
    std::cout << '\n';

    std::cout << "Please enter the author of the new site\n\n";
    yass_conf.author_name = ask_user("John Doe");
    std::cout << '\n';

    std::cout << "Please enter the contact email for the new site\n\n";
    yass_conf.author_email = ask_user("johndoe@example.com");
    std::cout << '\n';

    std::cout << "Please enter base URL of the new site\n\n";
    yass_conf.base_url = ask_user("http://localhost:8888");
    std::cout << '\n';

    std::cout << "Do you want to create Atom feed for the new site?\n";
    std::cout << "If yes, you must specify source for the feed: tags - create Atom\n";
    std::cout << "entries from proper tags in Markdown files, filename - the path\n";
    std::cout << "(related to the project directory path) to markdown file which\n";
    std::cout << "will be used as a source of atom feed (must have proper tags set\n";
    std::cout << "(inside). If you press Enter, creating Atom feed will be disabled\n\n";
    yass_conf.atom_feed_source = ask_user("none");
    std::cout << '\n';

    if (yass_conf.atom_feed_source != "none") {
        std::cout << "How much maximum entries should be in the Atom feed?\n";
        std::cout << "Recommended valuese are between 10 and 50.\n\n";
        yass_conf.atom_feed_amount = static_cast<std::uint32_t>(std::stoi(ask_user("25")));
    }

    std::cout << "Do you want to create sitemap file for the new site?\n\n";
    std::string answer_1 = ask_user("yes");
    std::cout << '\n';

    if (answer_1 == "yes" || answer_1 == "y" || answer_1.empty()) {
        yass_conf.sitemap_enabled = true;
    } else {
        yass_conf.sitemap_enabled = false;
    }

    std::cout << "Do you want to HTML embedded in your markdown to be converted?\n\n";
    std::string answer_2 = ask_user("yes");
    std::cout << '\n';

    if (answer_2 == "yes" || answer_2 == "y" || answer_2.empty()) {
        yass_conf.html_enabled = true;
    } else {
        yass_conf.html_enabled = false;
    }

    std::cout << "Do you want to set more technical options (like configuring\n";
    std::cout << "build-in web server)?\n\n";
    std::string answer_3 = ask_user("no");
    std::cout << '\n';

    if (answer_3 == "yes" || answer_3.empty()) {
        std::cout << "Should the program start web server when monitoring for\n";
        std::cout << "changes in the site?\n\n";
        std::string answer_4 = ask_user("yes");
        std::cout << '\n';

        if (answer_4 == "yes" || answer_4 == "y" || answer_4.empty()) {
            yass_conf.server_enabled = true;
        } else {
            yass_conf.server_enabled = false;
        }

        std::cout << "On which port should the web server listening?\n";
        std::cout << "Possible values are from 1 to 65535. Ports below 1025 require\n";
        std::cout << "root privileges.\n\n";
        yass_conf.server_port = static_cast<std::uint32_t>(std::stoi(ask_user("8888")));
        std::cout << '\n';

        std::cout << "Should whole monitoring option stop if encounter any error\n";
        std::cout << "during the site creation?\n\n";
        std::string answer_5 = ask_user("yes");
        std::cout << '\n';

        if (answer_5 == "yes" || answer_5 == "y" || answer_5.empty()) {
            yass_conf.stop_server_on_error = true;
        } else {
            yass_conf.stop_server_on_error = false;
        }

        std::cout << "Full path to the web broser which will be started when the\n";
        std::cout << "program starts in server mode.\n\n";
        yass_conf.browser_command = ask_user("none");
        std::cout << '\n';

        std::cout << "How often, in seconds, the program should check for changes\n";
        std::cout << "in the site files?\n\n";
        yass_conf.monitor_interval = std::chrono::duration<double>(std::stod(ask_user("5.0")));
        std::cout << '\n';

        std::cout << "How often, in seconds, the program should check for changes\n";
        std::cout << "in the site configuration file?\n\n";
        yass_conf.monitor_config_interval = std::chrono::duration<double>(std::stod(ask_user("60.0")));
        std::cout << '\n';
    }

    std::cout << "Do you want to set options related to compatybility with other\n";
    std::cout << "static sites generators?\n\n";
    std::string answer_6 = ask_user("no");
    std::cout << '\n';

    if (answer_6 == "yes" || answer_6 == "y") {
        std::cout << "What mark should be used as a start for template tag?\n\n";
        yass_conf.start_tag_separator = ask_user("{%");
        std::cout << '\n';

        std::cout << "What mark should be used as an end for template tag?\n\n";
        yass_conf.end_tag_separator = ask_user("%}");
        std::cout << '\n';

        std::cout << "What mark should be used as a start for the comment line in\n";
        std::cout << "Markdown files?\n\n";
        yass_conf.markdown_comment = ask_user("--");
    }
}

}  // namespace yass
