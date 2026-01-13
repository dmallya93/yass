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

#ifndef YASS_CONFIG_HPP
#define YASS_CONFIG_HPP

#include <chrono>
#include <filesystem>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

namespace config {

// Exception thrown when invalid data found in site config file
class InvalidConfigData : public std::runtime_error {
public:
  explicit InvalidConfigData(const std::string& message)
      : std::runtime_error(message) {}
};

// Data structure for parser settings
struct ParserConfig {
  std::filesystem::path output_directory{"_output"};
  std::filesystem::path layouts_directory{"_layouts"};
  std::filesystem::path modules_directory{"_modules"};
  std::vector<std::string> excluded_files{};
  bool server_enabled{true};
  int server_port{8888};
  std::chrono::duration<double> monitor_interval{5.0};
  std::string base_url{"http://localhost:8888"};
  bool sitemap_enabled{true};
  bool html_enabled{true};
  std::string atom_feed_source{"none"};
  std::string site_name{"New Site"};
  int atom_feed_amount{25};
  std::string markdown_comment{"--"};
  bool stop_server_on_error{false};
  std::string browser_command{"none"};
  std::chrono::duration<double> monitor_config_interval{60.0};
  std::string author_name{"John Doe"};
  std::string author_email{"johndoe@example.com"};
  std::string language{"en"};
  std::string start_tag_separator{"{%"};
  std::string end_tag_separator{"%}"};
  std::string description{"My new site"};
};

// Global configuration instance
extern ParserConfig yass_conf;

// Simple tags (key-value pairs)
extern std::unordered_map<std::string, std::string> site_tags;

// Directory where site files are
extern std::filesystem::path site_directory;

// Composite tags (key-vector of values)
extern std::unordered_map<std::string, std::vector<std::string>>
    global_table_tags;

// Create default config in directory with full path directory_name
void create_site_config(const std::filesystem::path& directory_name);

// Parse config file and set all settings and tags for site in directory
// with full path directory_name
void load_site_config(const std::filesystem::path& directory_name);

// Ask user for configuration (interactive mode)
void interactive_site_config();

} // namespace config

#endif // YASS_CONFIG_HPP
