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

#ifndef YASS_CONFIG_HPP
#define YASS_CONFIG_HPP

#include <chrono>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

namespace yass::config {

/// Exception thrown when invalid data is found in site config file
class Invalid_Config_Data : public std::runtime_error {
public:
  explicit Invalid_Config_Data(const std::string& message)
      : std::runtime_error(message) {}
};

/// Container type for storing excluded files list
using Excluded_Container = std::vector<std::string>;

/// Container type for storing simple string tags
using Tags_Container = std::unordered_map<std::string, std::string>;

/// Container type for storing composite/table tags (arrays of strings)
using Table_Tags_Container =
    std::unordered_map<std::string, std::vector<std::string>>;

/// Data structure for parser settings
///
/// Corresponds to Ada's Parser_Config record type from config.ads
struct Parser_Config {
  /// Path to directory with generated site (default: "_output")
  std::string output_directory = "_output";

  /// Path to directory where site layouts are (default: "_layouts")
  std::string layouts_directory = "_layouts";

  /// Path to directory where program modules are (default: "_modules")
  std::string modules_directory = "_modules";

  /// List of excluded files and directories from site
  Excluded_Container excluded_files;

  /// Is web server enabled (default: true)
  bool server_enabled = true;

  /// Port on which web server listens (default: 8888)
  int server_port = 8888;

  /// Interval (in seconds) to check for site changes (default: 5.0)
  std::chrono::duration<double> monitor_interval =
      std::chrono::duration<double>(5.0);

  /// Base URL for site, needed for sitemap (default: "http://localhost:8888")
  std::string base_url = "http://localhost:8888";

  /// Is sitemap creation enabled (default: true)
  bool sitemap_enabled = true;

  /// Is HTML passthrough in markdown enabled (default: true)
  bool html_enabled = true;

  /// Source of atom feed entries: "none", "tags", or filename (default: "none")
  std::string atom_feed_source = "none";

  /// Name of the site (default: "New Site")
  std::string site_name = "New Site";

  /// Number of entries in Atom feed (default: 25)
  int atom_feed_amount = 25;

  /// String used to mark comments in markdown files (default: "--")
  std::string markdown_comment = "--";

  /// Should server shut down on error (default: false)
  bool stop_server_on_error = false;

  /// Command to open web browser, "none" means don't open (default: "none")
  std::string browser_command = "none";

  /// Interval (in seconds) to check for config changes (default: 60.0)
  std::chrono::duration<double> monitor_config_interval =
      std::chrono::duration<double>(60.0);

  /// Author name for atom feed (default: "John Doe")
  std::string author_name = "John Doe";

  /// Author email for atom feed (default: "johndoe@example.com")
  std::string author_email = "johndoe@example.com";

  /// ISO language code for the site (default: "en")
  std::string language = "en";

  /// Start tag separator for template variables (default: "{%")
  std::string start_tag_separator = "{%";

  /// End tag separator for template variables (default: "%}")
  std::string end_tag_separator = "%}";

  /// Site description (default: "My new site")
  std::string description = "My new site";
};

/// Global parser configuration (corresponds to Ada's Yass_Conf)
extern Parser_Config yass_conf;

/// Global site tags (simple string key-value pairs)
extern Tags_Container site_tags;

/// Global site directory path
extern std::string site_directory;

/// Global table tags (composite tags with arrays of values)
extern Table_Tags_Container global_table_tags;

/// Create default config file in the specified directory
///
/// Generates a site.cfg file with all configuration fields documented
/// with comments. The file will be created at: directory_name/site.cfg
///
/// @param directory_name Full path to the directory where config file will be
/// created
/// @throws std::runtime_error if file creation fails
/// @pre directory_name must not be empty
void create_site_config(const std::string& directory_name);

/// Load and parse site config file from the specified directory
///
/// Parses site.cfg file and sets all settings and tags. The file is expected
/// at: directory_name/site.cfg
///
/// @param directory_name Full path to the directory from which config file
/// will be parsed
/// @throws Invalid_Config_Data if config contains invalid data
/// @throws std::runtime_error if file cannot be opened
/// @pre directory_name must not be empty
void load_site_config(const std::string& directory_name);

}  // namespace yass::config

#endif  // YASS_CONFIG_HPP
