// Copyright (c) 2019-2021 Bartek thindil Jasicki <thindil@laeran.pl>
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

#include "yass/config.hpp"

#include <algorithm>
#include <cctype>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

namespace yass {

// Global configuration variables
ParserConfig yass_conf = DEFAULT_PARSER_CONFIGURATION;
TagsContainer site_tags;
std::string site_directory;
TableTagsContainer global_table_tags;

namespace {

// Get platform-specific directory separator
constexpr char get_dir_separator() {
#ifdef _WIN32
  return '\\';
#else
  return '/';
#endif
}

constexpr char DIR_SEPARATOR = get_dir_separator();

// Trim whitespace from both ends of a string
std::string trim(const std::string& str) {
  auto start_iter =
      std::find_if(str.begin(), str.end(),
                   [](unsigned char ch) { return !std::isspace(ch); });
  auto end_iter =
      std::find_if(str.rbegin(), str.rend(),
                   [](unsigned char ch) { return !std::isspace(ch); })
          .base();
  return (start_iter < end_iter) ? std::string(start_iter, end_iter)
                                  : std::string{};
}

// Convert string to lowercase
std::string to_lower(const std::string& str) {
  std::string result = str;
  std::transform(result.begin(), result.end(), result.begin(),
                 [](unsigned char c) { return std::tolower(c); });
  return result;
}

// Split string by delimiter
std::vector<std::string> split(const std::string& str, char delimiter) {
  std::vector<std::string> tokens;
  std::stringstream ss(str);
  std::string token;
  while (std::getline(ss, token, delimiter)) {
    tokens.push_back(trim(token));
  }
  return tokens;
}

// Helper function to write a line to the config file
void put_line(std::ofstream& file, const std::string& line) {
  file << line << '\n';
}

// Normalize directory path (convert relative to absolute)
std::string normalize_dir(const std::string& directory_path,
                          const std::string& base_dir) {
  namespace fs = std::filesystem;
  fs::path path(directory_path);

  // If path is already absolute, return as-is
  if (path.is_absolute()) {
    return path.string();
  }

  // Make relative path absolute by prepending base directory
  fs::path base(base_dir);
  return (base / path).string();
}

}  // namespace

void create_site_config(const std::string& directory_name) {
  if (directory_name.empty()) {
    throw std::invalid_argument("Directory name cannot be empty");
  }

  std::string config_file_path =
      directory_name + DIR_SEPARATOR + "site.cfg";
  std::ofstream config_file(config_file_path);

  if (!config_file.is_open()) {
    throw std::runtime_error("Failed to create config file: " +
                             config_file_path);
  }

  // Convert boolean values to lowercase strings
  auto bool_to_str = [](bool value) -> std::string {
    return value ? "true" : "false";
  };

  put_line(config_file,
           "# Directory in which will be placed HTML files with site layout");
  put_line(config_file,
           "# (templates). May be absolute or relative to project directory.");
  put_line(config_file, "LayoutsDirectory = _layouts");
  put_line(config_file, "");

  put_line(config_file,
           "# Directory in which will be placed generated site. May be "
           "absolute");
  put_line(config_file, "# or relative to project directory.");
  put_line(config_file, "OutputDirectory = _output");
  put_line(config_file, "");

  put_line(
      config_file,
      "# Directory in which will be placed program modules used to generate");
  put_line(config_file,
           "# the site. May be absolute or relative to project directory.");
  put_line(config_file, "ModulesDirectory = _modules");
  put_line(config_file, "");

  put_line(
      config_file,
      "# List of excluded files and directories from list of sources used to");
  put_line(config_file, "# generating the site. All paths must be relative to "
                        "the project");
  put_line(config_file,
           "# directory. If you exclude directory, it whole content will be");
  put_line(config_file, "# excluded too. Layouts, modules and output "
                        "directories are excluded");
  put_line(config_file, "# by default.");
  put_line(config_file, "ExcludedFiles = .git,.gitignore,tags");
  put_line(config_file, "");

  put_line(config_file,
           "# The name of the site which will be created. If you have enabled");
  put_line(config_file,
           "# creating Atom feed then it is needed. Otherwise, you can use it "
           "as a");
  put_line(config_file, "# normal template tag.");
  put_line(config_file, "Name = " + yass_conf.site_name);
  put_line(config_file, "");

  put_line(config_file,
           "# The description of the site which will be created. Must be in "
           "one line,");
  put_line(
      config_file,
      "# no new line allowed. It is used to set meta tag description (which is");
  put_line(config_file,
           "# showed in search engines results) but only when pages don't set "
           "it.");
  put_line(config_file, "# Optional setting.");
  put_line(config_file, "Description = " + yass_conf.description);
  put_line(config_file, "");

  put_line(config_file,
           "# The ISO 639-1 language code in which the site will be created.");
  put_line(config_file, "Language = " + yass_conf.language);
  put_line(config_file, "");

  put_line(config_file,
           "# Name of author of the site. If you have enable creating Atom "
           "feed,");
  put_line(
      config_file,
      "# then it is needed. Otherwise, you can use it as a normal template tag.");
  put_line(config_file,
           "# It is also used in setting meta tag author for all pages.");
  put_line(config_file, "Author = " + yass_conf.author_name);
  put_line(config_file, "");

  put_line(
      config_file,
      "# Email address of author of the site. If you have enable creating Atom");
  put_line(config_file,
           "# feed, then it is needed. Otherwise, you can use it as a normal");
  put_line(config_file, "# template tag.");
  put_line(config_file, "AuthorEmail = " + yass_conf.author_email);
  put_line(config_file, "");

  put_line(
      config_file,
      "# Base URL of the site. It is needed mostly for creating sitemap and");
  put_line(config_file,
           "# Atom feed, but you can use it as a normal the site tag. If your "
           "site");
  put_line(
      config_file,
      "# will be available at https://mysite.com/blog then this will be your");
  put_line(config_file, "# BaseURL.");
  put_line(config_file, "BaseURL = " + yass_conf.base_url);
  put_line(config_file, "");

  put_line(
      config_file,
      "# Source which will be used for creating Atom feed of the site.");
  put_line(config_file,
           "# Possible values are: none: don't create atom feed, tags: create");
  put_line(config_file,
           "# Atom entries from proper tags in .md files, [filename]: the path");
  put_line(
      config_file,
      "# (related to the project directory path) to markdown file which will");
  put_line(config_file,
           "# be used as a source of atom feed (must have proper tags set "
           "inside).");
  put_line(config_file, "AtomFeedSource = " + yass_conf.atom_feed_source);
  put_line(config_file, "");

  put_line(
      config_file,
      "# Number of entries in the Atom feed of the site. Try not set it too");
  put_line(config_file,
           "# high, recommended values are between 10 and 50.");
  put_line(config_file,
           "AtomFeedAmount = " + std::to_string(yass_conf.atom_feed_amount));
  put_line(config_file, "");

  put_line(
      config_file,
      "# Should the program create sitemap when creating the site. Possible");
  put_line(config_file, "# values are true or false (case-insensitive).");
  put_line(config_file,
           "SitemapEnabled = " + bool_to_str(yass_conf.sitemap_enabled));
  put_line(config_file, "");

  put_line(
      config_file,
      "# Should program convert HTML in markdown documents to actual HTML.");
  put_line(config_file, "# Possible values are true or false (case-insensitive).");
  put_line(config_file,
           "HTMLEnabled = " + bool_to_str(yass_conf.html_enabled));
  put_line(config_file, "");

  put_line(
      config_file,
      "# Should the program start web server when monitoring for changes in");
  put_line(config_file, "# site. Possible values are true or false (case-insensitive).");
  put_line(config_file,
           "ServerEnabled = " + bool_to_str(yass_conf.server_enabled));
  put_line(config_file, "");

  put_line(
      config_file,
      "# Port on which web server will be listen if enabled. Possible values");
  put_line(
      config_file,
      "# are from 1 to 65535. Please remember, that ports below 1025 require");
  put_line(config_file, "# root privileges to work.");
  put_line(config_file,
           "ServerPort = " + std::to_string(yass_conf.server_port));
  put_line(config_file, "");

  put_line(
      config_file,
      "# Should web server and whole monitoring of the site changes stop if");
  put_line(
      config_file,
      "# encounter any error during the site creation.  Possible values are");
  put_line(config_file, "# true or false (case-insensitive).");
  put_line(
      config_file,
      "StopServerOnError = " + bool_to_str(yass_conf.stop_server_on_error));
  put_line(config_file, "");

  put_line(
      config_file,
      "# Full path to the command which will be used to start the web browser");
  put_line(
      config_file,
      R"(# with index.html page of the site. String "%s" (without quotes) will)");
  put_line(
      config_file,
      R"(# be replaced by server URL. If this setting is "none", the web)");
  put_line(config_file,
           "# browser will be not started, same as when the web server is "
           "disabled.");
  put_line(config_file, "BrowserCommand = " + yass_conf.browser_command);
  put_line(config_file, "");

  put_line(
      config_file,
      "# How often (in seconds) the program should monitor site for changes");
  put_line(config_file,
           "# and regenerate it if needed. Can be any positive number, but you");
  put_line(
      config_file,
      "# probably don't want to set it to check every few thousands years :)");
  put_line(config_file, "MonitorInterval = " +
                            std::to_string(yass_conf.monitor_interval.count()));
  put_line(config_file, "");

  put_line(
      config_file,
      "# How often (in seconds) the program should monitor site configuration");
  put_line(
      config_file,
      "# for changes and reconfigure it if needed. Can be any positive number.");
  put_line(
      config_file,
      "MonitorConfigInterval = " +
          std::to_string(yass_conf.monitor_config_interval.count()));
  put_line(config_file, "");

  put_line(
      config_file,
      "# String used to mark start of the templates tags, used in templates");
  put_line(
      config_file,
      "# files. You may want to change it, if you want to use templates from");
  put_line(config_file, "# other static site generator.");
  put_line(config_file, "StartTagSeparator = " + yass_conf.start_tag_separator);
  put_line(config_file, "");

  put_line(config_file,
           "# String used to mark end of the templates tags, used in templates");
  put_line(
      config_file,
      "# files. You may want to change it, if you want to use templates from");
  put_line(config_file, "# other static site generator.");
  put_line(config_file, "EndTagSeparator = " + yass_conf.end_tag_separator);
  put_line(config_file, "");

  put_line(
      config_file,
      "# String used to mark comments in markdown files which will be parsed.");
  put_line(config_file, "MarkdownComment = " + yass_conf.markdown_comment);
  put_line(config_file, "");

  put_line(config_file,
           "# Site tags, optional. Tags can be 4 types: strings, boolean, "
           "numeric");
  put_line(config_file, "# or composite.");
  put_line(
      config_file,
      "# First 3 types of tags are in Name = Value scheme. For strings, it can");
  put_line(
      config_file,
      "# be any alphanumeric value without new line sign. For boolean it must");
  put_line(
      config_file,
      R"(# be "true" or "false", for numeric any number. Program will detect)");
  put_line(config_file,
           "# self which type of tag is and properly set it. It always falls "
           "back");
  put_line(config_file, "# to string value.");
  put_line(
      config_file,
      "# Composite tags first must be initialized with Name = [] then just");
  put_line(
      config_file,
      "# add as many as you want values to it by Name = Value scheme.");
  put_line(config_file,
           "# For more information about site.cfg file please check program");
  put_line(config_file, "# documentation.");
  put_line(config_file, "");

  config_file.close();
}

void load_site_config(const std::string& directory_name) {
  if (directory_name.empty()) {
    throw std::invalid_argument("Directory name cannot be empty");
  }

  // Clear global containers
  site_tags.clear();
  global_table_tags.clear();

  std::string config_path = directory_name + "/site.cfg";
  std::ifstream config_file(config_path);

  if (!config_file.is_open()) {
    throw std::runtime_error("Failed to open config file: " + config_path);
  }

  std::string line;
  std::string raw_data;

  while (std::getline(config_file, line)) {
    raw_data = trim(line);

    // Skip empty lines and comments
    if (raw_data.empty() || raw_data[0] == '#') {
      continue;
    }

    // Find the equals sign
    size_t equal_pos = raw_data.find('=');
    if (equal_pos == std::string::npos) {
      throw InvalidConfigData(raw_data);
    }

    // Extract field name and value (with proper trimming)
    std::string field_name = trim(raw_data.substr(0, equal_pos));
    std::string value = trim(raw_data.substr(equal_pos + 1));

    try {
      if (field_name == "LayoutsDirectory") {
        yass_conf.layouts_directory = value;

      } else if (field_name == "OutputDirectory") {
        yass_conf.output_directory = value;

      } else if (field_name == "ModulesDirectory") {
        yass_conf.modules_directory = value;

      } else if (field_name == "ExcludedFiles") {
        std::vector<std::string> files = split(value, ',');
        yass_conf.excluded_files.insert(yass_conf.excluded_files.end(),
                                        files.begin(), files.end());

      } else if (field_name == "ServerEnabled") {
        yass_conf.server_enabled = (to_lower(value) == "true");

      } else if (field_name == "ServerPort") {
        auto port_value = std::stoul(value);
        if (port_value > 65535) {
          throw InvalidConfigData(raw_data);
        }
        yass_conf.server_port = static_cast<std::uint32_t>(port_value);

      } else if (field_name == "StopServerOnError") {
        yass_conf.stop_server_on_error = (to_lower(value) == "true");

      } else if (field_name == "BrowserCommand") {
        // Replace %s with server URL
        size_t placeholder_pos = value.find("%s");
        if (placeholder_pos != std::string::npos) {
          value.replace(placeholder_pos, 2,
                        "http://localhost:" + std::to_string(yass_conf.server_port));
        }
        yass_conf.browser_command = value;

      } else if (field_name == "MonitorInterval") {
        yass_conf.monitor_interval = std::chrono::duration<double>(std::stod(value));

      } else if (field_name == "MonitorConfigInterval") {
        yass_conf.monitor_config_interval =
            std::chrono::duration<double>(std::stod(value));

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
          // Convert .md extension to .html
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
        // Generic site tag
        site_tags[field_name] = value;
      }
    } catch (const std::exception&) {
      throw InvalidConfigData(raw_data);
    }
  }

  config_file.close();

  // Normalize directory paths
  yass_conf.layouts_directory =
      normalize_dir(yass_conf.layouts_directory, directory_name);
  yass_conf.output_directory =
      normalize_dir(yass_conf.output_directory, directory_name);
  yass_conf.modules_directory =
      normalize_dir(yass_conf.modules_directory, directory_name);

  // Add default excluded files
  yass_conf.excluded_files.emplace_back(".");
  yass_conf.excluded_files.emplace_back("..");
  yass_conf.excluded_files.emplace_back("site.cfg");

  // Add layout, output, and module directory names to excluded list
  namespace fs = std::filesystem;
  yass_conf.excluded_files.emplace_back(
      fs::path(yass_conf.layouts_directory).filename().string());
  yass_conf.excluded_files.emplace_back(
      fs::path(yass_conf.output_directory).filename().string());
  yass_conf.excluded_files.emplace_back(
      fs::path(yass_conf.modules_directory).filename().string());

  // Store site directory
  site_directory = directory_name;
}

void interactive_site_config() {
  auto ask_user = [](const std::string& default_value) -> std::string {
    std::cout << "(Default - " << default_value << ") > ";
    std::string answer;
    std::getline(std::cin, answer);
    return answer.empty() ? default_value : answer;
  };

  std::cout << "Now we ask you some questions about your new site.\n";
  std::cout << "You can always change it later by modifying the site "
               "configuration\n";
  std::cout << "file. If you just press Enter as a answer, default value will "
               "be\n";
  std::cout << "used.\n\n";

  std::cout << "Please enter the name of the new site\n\n";
  yass_conf.site_name = ask_user("New Site");
  std::cout << '\n';

  std::cout << "Please enter the description of the new site.\n";
  std::cout << "It is used to create meta tag for the website (which is "
               "showed\n";
  std::cout << "in search engines results) but only if pages don't set own.\n";
  std::cout << "Must be set in one line, no new line allowed.\n\n";
  yass_conf.description = ask_user("My new site");
  std::cout << '\n';

  std::cout << "Please enter language code in which the new site will be "
               "written\n\n";
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
  std::cout << "If yes, you must specify source for the feed: tags - create "
               "Atom\n";
  std::cout << "entries from proper tags in Markdown files, filename - the "
               "path\n";
  std::cout << "(related to the project directory path) to markdown file "
               "which\n";
  std::cout << "will be used as a source of atom feed (must have proper tags "
               "set\n";
  std::cout << "(inside). If you press Enter, creating Atom feed will be "
               "disabled\n\n";
  yass_conf.atom_feed_source = ask_user("none");
  std::cout << '\n';

  if (yass_conf.atom_feed_source != "none") {
    std::cout << "How much maximum entries should be in the Atom feed?\n";
    std::cout << "Recommended valuese are between 10 and 50.\n\n";
    yass_conf.atom_feed_amount = static_cast<std::uint32_t>(std::stoul(ask_user("25")));
  }

  std::cout << "Do you want to create sitemap file for the new site?\n\n";
  std::string answer = ask_user("yes");
  std::cout << '\n';
  yass_conf.sitemap_enabled =
      (answer == "yes" || answer == "y" || answer.empty());

  std::cout << "Do you want to HTML embedded in your markdown to be "
               "converted?\n\n";
  answer = ask_user("yes");
  std::cout << '\n';
  yass_conf.html_enabled = (answer == "yes" || answer == "y" || answer.empty());

  std::cout << "Do you want to set more technical options (like configuring\n";
  std::cout << "build-in web server)?\n\n";
  answer = ask_user("no");
  std::cout << '\n';

  if (answer == "yes" || answer.empty()) {
    std::cout << "Should the program start web server when monitoring for\n";
    std::cout << "changes in the site?\n\n";
    answer = ask_user("yes");
    std::cout << '\n';
    yass_conf.server_enabled =
        (answer == "yes" || answer == "y" || answer.empty());

    std::cout << "On which port should the web server listening?\n";
    std::cout << "Possible values are from 1 to 65535. Ports below 1025 "
                 "require\n";
    std::cout << "root privileges.\n\n";
    yass_conf.server_port = static_cast<std::uint32_t>(std::stoul(ask_user("8888")));
    std::cout << '\n';

    std::cout << "Should whole monitoring option stop if encounter any error\n";
    std::cout << "during the site creation?\n\n";
    answer = ask_user("yes");
    std::cout << '\n';
    yass_conf.stop_server_on_error =
        (answer == "yes" || answer == "y" || answer.empty());

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
    yass_conf.monitor_config_interval =
        std::chrono::duration<double>(std::stod(ask_user("60.0")));
    std::cout << '\n';
  }

  std::cout << "Do you want to set options related to compatybility with "
               "other\n";
  std::cout << "static sites generators?\n\n";
  answer = ask_user("no");
  std::cout << '\n';

  if (answer == "yes" || answer == "y") {
    std::cout << "What mark should be used as a start for template tag?\n\n";
    yass_conf.start_tag_separator = ask_user("{%");
    std::cout << '\n';

    std::cout << "What mark should be used as an end for template tag?\n\n";
    yass_conf.end_tag_separator = ask_user("%}");
    std::cout << '\n';

    std::cout << "What mark should be used as a start for the comment line "
                 "in\n";
    std::cout << "Markdown files?\n\n";
    yass_conf.markdown_comment = ask_user("--");
  }
}

}  // namespace yass
