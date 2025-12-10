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

#ifndef YASS_CONFIG_HPP
#define YASS_CONFIG_HPP

#include <chrono>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

namespace yass {

/// Exception thrown when invalid data is found in site configuration file
class InvalidConfigData : public std::runtime_error {
 public:
  explicit InvalidConfigData(const std::string& line)
      : std::runtime_error("Invalid configuration data: " + line) {}
};

/// Container for excluded files and directories
using ExcludedContainer = std::vector<std::string>;

/// Container for AWS template tags (simple string key-value pairs)
using TagsContainer = std::unordered_map<std::string, std::string>;

/// Container for AWS template table tags (vector of strings for each key)
using TableTagsContainer =
    std::unordered_map<std::string, std::vector<std::string>>;

/// Data structure for parser settings
struct ParserConfig {
  /// Path to directory with generated site
  std::string output_directory = "_output";

  /// Path to directory where site layouts are
  std::string layouts_directory = "_layouts";

  /// Path to directory where program modules for this site are
  std::string modules_directory = "_modules";

  /// List of excluded files and directories from site
  ExcludedContainer excluded_files;

  /// Is web server enabled
  bool server_enabled = true;

  /// Port on which web server listens
  std::uint32_t server_port = 8888;

  /// Interval (in seconds) for checking site changes
  std::chrono::duration<double> monitor_interval{5.0};

  /// Base URL for site (needed for sitemap)
  std::string base_url = "http://localhost:8888";

  /// Is creating sitemap enabled
  bool sitemap_enabled = true;

  /// Should HTML in markdown be converted
  bool html_enabled = true;

  /// Source of atom feed entries
  /// Possible values: "none" (don't create), "tags" (from markdown tags),
  /// or filename path (markdown file as source)
  std::string atom_feed_source = "none";

  /// Name of the site
  std::string site_name = "New Site";

  /// Number of entries in the Atom feed
  std::uint32_t atom_feed_amount = 25;

  /// String used to mark comments in markdown files
  std::string markdown_comment = "--";

  /// Should server stop on error during site creation
  bool stop_server_on_error = false;

  /// Command used to open web browser (empty = don't open)
  std::string browser_command = "none";

  /// Interval (in seconds) for checking config changes
  std::chrono::duration<double> monitor_config_interval{60.0};

  /// Name of author of the site
  std::string author_name = "John Doe";

  /// Email address of author of the site
  std::string author_email = "johndoe@example.com";

  /// ISO code of the language of the site
  std::string language = "en";

  /// String used to mark start of template tags
  std::string start_tag_separator = "{%";

  /// String used to mark end of template tags
  std::string end_tag_separator = "%}";

  /// Site description
  std::string description = "My new site";
};

/// Default parser configuration values
inline const ParserConfig DEFAULT_PARSER_CONFIGURATION{};

/// Global settings for the program
extern ParserConfig yass_conf;

/// Global site tags (like title, author, etc)
extern TagsContainer site_tags;

/// Directory where site files are
extern std::string site_directory;

/// Global table tags, used in @@TABLE@@ statement
extern TableTagsContainer global_table_tags;

/// Create default config in directory with full path directory_name
///
/// @param directory_name Full path to the directory where config file will be
/// created
/// @throws std::invalid_argument if directory_name is empty
/// @throws std::runtime_error if file creation fails
void create_site_config(const std::string& directory_name);

/// Parse config file and set all settings and tags for site in directory
/// with full path directory_name
///
/// @param directory_name Full path to the directory from which config file
/// will be parsed
/// @throws std::invalid_argument if directory_name is empty
/// @throws InvalidConfigData if configuration parsing fails
/// @throws std::runtime_error if file reading fails
void load_site_config(const std::string& directory_name);

/// Ask user for configuration interactively
void interactive_site_config();

}  // namespace yass

#endif  // YASS_CONFIG_HPP
