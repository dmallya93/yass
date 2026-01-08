#ifndef YASS_CONFIG_HPP
#define YASS_CONFIG_HPP

#include <filesystem>
#include <map>
#include <string>
#include <vector>

namespace config {

// Configuration structure matching Ada's Parser_Config
struct YassConfig {
  // Directory settings
  std::string layouts_directory = "_layouts";
  std::string output_directory = "_output";
  std::string modules_directory = "_modules";

  // Server settings
  bool server_enabled = true;
  int server_port = 8888;
  bool stop_server_on_error = true;
  std::string browser_command = "none";

  // Site metadata
  std::string site_name = "New Site";
  std::string site_description = "My new site";
  std::string language = "en";
  std::string author = "John Doe";
  std::string author_email = "johndoe@example.com";

  // Feed and sitemap settings
  std::string atom_feed_source = "none";
  int atom_feed_amount = 25;
  bool sitemap_enabled = true;
  bool html_enabled = true;

  // Monitoring settings
  double monitor_interval = 5.0;
  double monitor_config_interval = 60.0;

  // Template settings
  std::string start_tag_separator = "{%";
  std::string end_tag_separator = "%}";
  std::string markdown_comment = "--";

  // Base URL
  std::string base_url = "http://localhost:8888";

  // Excluded files list
  std::vector<std::string> excluded_files;

  // User-defined tags
  std::map<std::string, std::string> tags;
  std::map<std::string, std::vector<std::string>> table_tags;
};

// Global configuration instance (equivalent to Ada's Yass_Conf)
extern YassConfig yass_config;

// Global site directory path (set when loading config)
extern std::filesystem::path site_directory;

// Create a default site.cfg file in the specified directory
// directory_name: The directory where site.cfg will be created
void create_site_config(const std::filesystem::path& directory_name);

// Interactively prompt user for site configuration values
// Updates the global yass_config with user input
void interactive_site_config();

// Load site configuration from site.cfg file
// directory_name: The directory containing the site.cfg file
// Returns true on success, false on error
bool load_site_config(const std::filesystem::path& directory_name);

}  // namespace config

#endif  // YASS_CONFIG_HPP
