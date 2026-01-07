// Copyright (c) 2019-2021 Bartek thindil Jasicki <thindil@laeran.pl>
// Copyright (c) 2022-2024 A.J. Ianozi
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

#include "yass/config.hpp"

#include <algorithm>
#include <cctype>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <stdexcept>

namespace yass::config {

// Define global variables
Parser_Config yass_conf;
Tags_Container site_tags;
std::string site_directory;
Table_Tags_Container global_table_tags;

namespace {

// Directory separator for current platform
#ifdef _WIN32
constexpr char DIR_SEPARATOR = '\\';
#else
constexpr char DIR_SEPARATOR = '/';
#endif

/// Convert string to lowercase
std::string to_lower(const std::string& str) {
  std::string result = str;
  std::transform(result.begin(), result.end(), result.begin(),
                 [](unsigned char c) { return std::tolower(c); });
  return result;
}

/// Trim whitespace from both ends of string
std::string trim(const std::string& str) {
  const auto start = std::find_if_not(
      str.begin(), str.end(), [](unsigned char c) { return std::isspace(c); });
  const auto end =
      std::find_if_not(str.rbegin(), str.rend(), [](unsigned char c) {
        return std::isspace(c);
      }).base();
  return (start < end) ? std::string(start, end) : std::string();
}

/// Split string by delimiter
std::vector<std::string> split(const std::string& str, char delimiter) {
  std::vector<std::string> tokens;
  std::stringstream ss(str);
  std::string token;
  while (std::getline(ss, token, delimiter)) {
    token = trim(token);
    if (!token.empty()) {
      tokens.push_back(token);
    }
  }
  return tokens;
}

/// Normalize directory path to absolute if it's relative
void normalize_dir(std::string& directory_path,
                   const std::string& base_directory) {
  if (directory_path.empty()) {
    return;
  }

  // Check if path is already absolute
  bool is_absolute = false;
#ifdef _WIN32
  // On Windows, check for drive letter (e.g., C:)
  if (directory_path.length() >= 2 && directory_path[1] == ':') {
    is_absolute = true;
  }
#else
  // On Unix-like systems, check for leading /
  if (directory_path[0] == '/') {
    is_absolute = true;
  }
#endif

  if (!is_absolute) {
    directory_path = base_directory + DIR_SEPARATOR + directory_path;
  }
}

}  // anonymous namespace

void create_site_config(const std::string& directory_name,
                       const Parser_Config& config) {
  if (directory_name.empty()) {
    throw std::invalid_argument("directory_name must not be empty");
  }

  const std::string config_path = directory_name + DIR_SEPARATOR + "site.cfg";

  std::ofstream config_file(config_path, std::ios::app);
  if (!config_file) {
    throw std::runtime_error("Failed to create config file: " + config_path);
  }

  // Helper lambda to write lines
  auto pl = [&config_file](const std::string& line) {
    config_file << line << '\n';
  };

  pl("# Directory in which will be placed HTML files with site layout");
  pl("# (templates). May be absolute or relative to project directory.");
  pl("LayoutsDirectory = _layouts");
  pl("");

  pl("# Directory in which will be placed generated site. May be absolute");
  pl("# or relative to project directory.");
  pl("OutputDirectory = _output");
  pl("");

  pl("# Directory in which will be placed program modules used to generate");
  pl("# the site. May be absolute or relative to project directory.");
  pl("ModulesDirectory = _modules");
  pl("");

  pl("# List of excluded files and directories from list of sources used to");
  pl("# generating the site. All paths must be relative to the project");
  pl("# directory. If you exclude directory, it whole content will be");
  pl("# excluded too. Layouts, modules and output directories are excluded");
  pl("# by default.");
  pl("ExcludedFiles = .git,.gitignore,tags");
  pl("");

  pl("# The name of the site which will be created. If you have enabled");
  pl("# creating Atom feed then it is needed. Otherwise, you can use it as a");
  pl("# normal template tag.");
  pl("Name = " + config.site_name);
  pl("");

  pl("# The description of the site which will be created. Must be in one "
     "line,");
  pl("# no new line allowed. It is used to set meta tag description (which is");
  pl("# showed in search engines results) but only when pages don't set it.");
  pl("# Optional setting.");
  pl("Description = " + config.description);
  pl("");

  pl("# The ISO 639-1 language code in which the site will be created.");
  pl("Language = " + config.language);
  pl("");

  pl("# Name of author of the site. If you have enable creating Atom feed,");
  pl("# then it is needed. Otherwise, you can use it as a normal template "
     "tag.");
  pl("# It is also used in setting meta tag author for all pages.");
  pl("Author = " + config.author_name);
  pl("");

  pl("# Email address of author of the site. If you have enable creating Atom");
  pl("# feed, then it is needed. Otherwise, you can use it as a normal");
  pl("# template tag.");
  pl("AuthorEmail = " + config.author_email);
  pl("");

  pl("# Base URL of the site. It is needed mostly for creating sitemap and");
  pl("# Atom feed, but you can use it as a normal the site tag. If your site");
  pl("# will be available at https://mysite.com/blog then this will be your");
  pl("# BaseURL.");
  pl("BaseURL = " + config.base_url);
  pl("");

  pl("# Source which will be used for creating Atom feed of the site.");
  pl("# Possible values are: none: don't create atom feed, tags: create");
  pl("# Atom entries from proper tags in .md files, [filename]: the path");
  pl("# (related to the project directory path) to markdown file which will");
  pl("# be used as a source of atom feed (must have proper tags set inside).");
  pl("AtomFeedSource = " + config.atom_feed_source);
  pl("");

  pl("# Number of entries in the Atom feed of the site. Try not set it too");
  pl("# high, recommended values are between 10 and 50.");
  pl("AtomFeedAmount = " + std::to_string(config.atom_feed_amount));
  pl("");

  pl("# Should the program create sitemap when creating the site. Possible");
  pl("# values are true or false (case-insensitive).");
  pl("SitemapEnabled = " +
     std::string(config.sitemap_enabled ? "true" : "false"));
  pl("");

  pl("# Should program convert HTML in markdown documents to actual HTML.");
  pl("# Possible values are true or false (case-insensitive).");
  pl("HTMLEnabled = " + std::string(config.html_enabled ? "true" : "false"));
  pl("");

  pl("# Should the program start web server when monitoring for changes in");
  pl("# site. Possible values are true or false (case-insensitive).");
  pl("ServerEnabled = " +
     std::string(config.server_enabled ? "true" : "false"));
  pl("");

  pl("# Port on which web server will be listen if enabled. Possible values");
  pl("# are from 1 to 65535. Please remember, that ports below 1025 require");
  pl("# root privileges to work.");
  pl("ServerPort = " + std::to_string(config.server_port));
  pl("");

  pl("# Should web server and whole monitoring of the site changes stop if");
  pl("# encounter any error during the site creation.  Possible values are");
  pl("# true or false (case-insensitive).");
  pl("StopServerOnError = " +
     std::string(config.stop_server_on_error ? "true" : "false"));
  pl("");

  pl("# Full path to the command which will be used to start the web browser");
  pl("# with index.html page of the site. String \"%s\" (without quotes) will");
  pl("# be replaced by server URL. If this setting is \"none\", the web");
  pl("# browser will be not started, same as when the web server is disabled.");
  pl("BrowserCommand = " + config.browser_command);
  pl("");

  pl("# How often (in seconds) the program should monitor site for changes");
  pl("# and regenerate it if needed. Can be any positive number, but you");
  pl("# probably don't want to set it to check every few thousands years :)");
  pl("MonitorInterval = " + std::to_string(config.monitor_interval.count()));
  pl("");

  pl("# How often (in seconds) the program should monitor site configuration");
  pl("# for changes and reconfigure it if needed. Can be any positive number.");
  pl("MonitorConfigInterval = " +
     std::to_string(config.monitor_config_interval.count()));
  pl("");

  pl("# String used to mark start of the templates tags, used in templates");
  pl("# files. You may want to change it, if you want to use templates from");
  pl("# other static site generator.");
  pl("StartTagSeparator = " + config.start_tag_separator);
  pl("");

  pl("# String used to mark end of the templates tags, used in templates");
  pl("# files. You may want to change it, if you want to use templates from");
  pl("# other static site generator.");
  pl("EndTagSeparator = " + config.end_tag_separator);
  pl("");

  pl("# String used to mark comments in markdown files which will be parsed.");
  pl("MarkdownComment = " + config.markdown_comment);
  pl("");

  pl("# Site tags, optional. Tags can be 4 types: strings, boolean, numeric");
  pl("# or composite.");
  pl("# First 3 types of tags are in Name = Value scheme. For strings, it can");
  pl("# be any alphanumeric value without new line sign. For boolean it must");
  pl("# be \"true\" or \"false\", for numeric any number. Program will detect");
  pl("# self which type of tag is and properly set it. It always falls back");
  pl("# to string value.");
  pl("# Composite tags first must be initialized with Name = [] then just");
  pl("# add as many as you want values to it by Name = Value scheme.");
  pl("# For more information about site.cfg file please check program");
  pl("# documentation.");
  pl("");

  config_file.close();
}

void load_site_config(const std::string& directory_name) {
  if (directory_name.empty()) {
    throw std::invalid_argument("directory_name must not be empty");
  }

  // Clear existing tags
  site_tags.clear();
  global_table_tags.clear();

  const std::string config_path = directory_name + "/site.cfg";

  std::ifstream config_file(config_path);
  if (!config_file) {
    throw std::runtime_error("Failed to open config file: " + config_path);
  }

  std::string line;
  while (std::getline(config_file, line)) {
    // Skip empty lines and comments
    line = trim(line);
    if (line.empty() || line[0] == '#') {
      continue;
    }

    // Find the = delimiter
    const size_t equal_pos = line.find('=');
    if (equal_pos == std::string::npos) {
      throw Invalid_Config_Data("Missing '=' delimiter in line: " + line);
    }

    // Extract field name and value
    std::string field_name = trim(line.substr(0, equal_pos));
    std::string value = trim(line.substr(equal_pos + 1));

    // Parse configuration fields
    if (field_name == "LayoutsDirectory") {
      yass_conf.layouts_directory = value;

    } else if (field_name == "OutputDirectory") {
      yass_conf.output_directory = value;

    } else if (field_name == "ModulesDirectory") {
      yass_conf.modules_directory = value;

    } else if (field_name == "ExcludedFiles") {
      // Split by comma and add to excluded files vector
      auto files = split(value, ',');
      for (const auto& file : files) {
        yass_conf.excluded_files.push_back(file);
      }

    } else if (field_name == "ServerEnabled") {
      yass_conf.server_enabled = (to_lower(value) == "true");

    } else if (field_name == "ServerPort") {
      try {
        yass_conf.server_port = std::stoi(value);
        if (yass_conf.server_port > 65535) {
          throw Invalid_Config_Data("ServerPort must be <= 65535: " + line);
        }
      } catch (const std::exception&) {
        throw Invalid_Config_Data("Invalid ServerPort value: " + line);
      }

    } else if (field_name == "StopServerOnError") {
      yass_conf.stop_server_on_error = (to_lower(value) == "true");

    } else if (field_name == "BrowserCommand") {
      // Replace %s with actual server URL if present
      const size_t pos = value.find("%s");
      if (pos != std::string::npos) {
        std::string url =
            "http://localhost:" + std::to_string(yass_conf.server_port);
        value.replace(pos, 2, url);
      }
      yass_conf.browser_command = value;

    } else if (field_name == "MonitorInterval") {
      try {
        double interval_seconds = std::stod(value);
        yass_conf.monitor_interval =
            std::chrono::duration<double>(interval_seconds);
      } catch (const std::exception&) {
        throw Invalid_Config_Data("Invalid MonitorInterval value: " + line);
      }

    } else if (field_name == "MonitorConfigInterval") {
      try {
        double interval_seconds = std::stod(value);
        yass_conf.monitor_config_interval =
            std::chrono::duration<double>(interval_seconds);
      } catch (const std::exception&) {
        throw Invalid_Config_Data("Invalid MonitorConfigInterval value: " +
                                  line);
      }

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
        // Convert .md to .html for feed source file
        if (value.length() > 3 && value.substr(value.length() - 3) == ".md") {
          yass_conf.atom_feed_source =
              value.substr(0, value.length() - 3) + ".html";
        } else {
          yass_conf.atom_feed_source = value;
        }
      }

    } else if (field_name == "AtomFeedAmount") {
      try {
        yass_conf.atom_feed_amount = std::stoi(value);
      } catch (const std::exception&) {
        throw Invalid_Config_Data("Invalid AtomFeedAmount value: " + line);
      }

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
      // Initialize composite/table tag
      global_table_tags[field_name] = std::vector<std::string>();

    } else if (global_table_tags.find(field_name) != global_table_tags.end()) {
      // Append to existing composite tag
      global_table_tags[field_name].push_back(value);

    } else {
      // Custom site tag
      site_tags[field_name] = value;
    }
  }

  config_file.close();

  // Normalize directory paths (convert relative to absolute)
  normalize_dir(yass_conf.layouts_directory, directory_name);
  normalize_dir(yass_conf.output_directory, directory_name);
  normalize_dir(yass_conf.modules_directory, directory_name);

  // Add default excluded files (matching Ada implementation)
  yass_conf.excluded_files.push_back(".");
  yass_conf.excluded_files.push_back("..");
  yass_conf.excluded_files.push_back("site.cfg");

  // Extract just the directory names (not full paths) for exclusion
  yass_conf.excluded_files.push_back(
      std::filesystem::path(yass_conf.layouts_directory).filename().string());
  yass_conf.excluded_files.push_back(
      std::filesystem::path(yass_conf.output_directory).filename().string());
  yass_conf.excluded_files.push_back(
      std::filesystem::path(yass_conf.modules_directory).filename().string());

  // Store site directory
  site_directory = directory_name;
}

}  // namespace yass::config
