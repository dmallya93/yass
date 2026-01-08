#include "config.hpp"

#include <algorithm>
#include <cctype>
#include <fstream>
#include <iostream>
#include <limits>
#include <sstream>
#include <string>

#include "messages.hpp"

namespace config {

// Global configuration instance
YassConfig yass_config;

// Global site directory path
std::filesystem::path site_directory;

// Helper function to write a line to the config file
static void write_line(std::ofstream& file, const std::string& line) {
  file << line << '\n';
}

void create_site_config(const std::filesystem::path& directory_name) {
  std::filesystem::path config_path = directory_name / "site.cfg";
  std::ofstream config_file(config_path);

  if (!config_file) {
    throw std::runtime_error("Failed to create site.cfg at: " +
                             config_path.string());
  }

  // Write configuration file with all default values and comments
  write_line(config_file, "-- All lines which starts with double minus sign "
                          "are comments and they");
  write_line(config_file, "-- are ignored by the program.");
  write_line(config_file, "");
  write_line(config_file, "-- Name of the directory where layouts for the "
                          "site are. Path relative to");
  write_line(config_file, "-- the site directory. Default value is _layouts");
  write_line(config_file,
             "LayoutsDirectory: " + yass_config.layouts_directory);
  write_line(config_file, "");
  write_line(config_file, "-- Name of the directory where output files for "
                          "the site are. Path relative");
  write_line(config_file, "-- to the site directory. Default value is _output");
  write_line(config_file, "OutputDirectory: " + yass_config.output_directory);
  write_line(config_file, "");
  write_line(config_file, "-- Name of the directory where modules for the "
                          "site are. Path relative");
  write_line(config_file, "-- to the site directory. Default value is _modules");
  write_line(config_file,
             "ModulesDirectory: " + yass_config.modules_directory);
  write_line(config_file, "");
  write_line(config_file, "-- Name for the site");
  write_line(config_file, "Name: " + yass_config.site_name);
  write_line(config_file, "");
  write_line(config_file,
             "-- Description of the site. This will be used in meta tag "
             "description in HTML");
  write_line(config_file, "Description: " + yass_config.site_description);
  write_line(config_file, "");
  write_line(
      config_file,
      "-- Base URL for the site. Needed to generate correct sitemap and tags");
  write_line(config_file, "-- for header of HTML. Default value is: none");
  write_line(config_file, "BaseUrl: " + yass_config.base_url);
  write_line(config_file, "");
  write_line(
      config_file,
      "-- Language code for the site. This will be used in HTML header. Default");
  write_line(config_file, "-- value is en");
  write_line(config_file, "Language: " + yass_config.language);
  write_line(config_file, "");
  write_line(config_file,
             "-- Name of the author of the site. This will be used in meta "
             "tag author");
  write_line(config_file, "-- in HTML. Default value is unknown");
  write_line(config_file, "Author: " + yass_config.author);
  write_line(config_file, "");
  write_line(config_file, "-- Email address to the author of the site.");
  write_line(config_file, "AuthorEmail: " + yass_config.author_email);
  write_line(config_file, "");
  write_line(config_file, "-- Source of Atom feed. Possible options are:");
  write_line(config_file,
             "-- none - no Atom feed will be generated (default value)");
  write_line(config_file,
             "-- tags - Atom feed will be generated from all tags with name "
             "\"title\" for");
  write_line(config_file, "--        chosen file");
  write_line(config_file, "-- filename.md - Atom feed will be generated from "
                          "\"title\" tags in chosen");
  write_line(config_file, "--               file");
  write_line(config_file, "AtomFeedSource: " + yass_config.atom_feed_source);
  write_line(config_file, "");
  write_line(
      config_file,
      "-- Amount of entries in Atom feed. Default value is 25. Values below 1");
  write_line(config_file, "-- are treated as default value");
  write_line(config_file, "AtomFeedAmount: " +
                              std::to_string(yass_config.atom_feed_amount));
  write_line(config_file, "");
  write_line(
      config_file,
      "-- If true, the program will create sitemap.xml from all md files. If");
  write_line(config_file, "-- false, the program will not generate "
                          "sitemap.xml. Default value is");
  write_line(config_file, "-- true");
  write_line(config_file, "SitemapEnabled: " +
                              std::string(yass_config.sitemap_enabled ? "true"
                                                                       : "false"));
  write_line(config_file, "");
  write_line(
      config_file,
      "-- If true, the program will convert all HTML code in Markdown to proper");
  write_line(config_file, "-- HTML output. If false, all HTML code will be "
                          "escaped. Default value is");
  write_line(config_file, "-- true");
  write_line(config_file, "HtmlEnabled: " +
                              std::string(yass_config.html_enabled ? "true"
                                                                    : "false"));
  write_line(config_file, "");
  write_line(config_file, "-- If true, the program will start the HTTP "
                          "server on selected port");
  write_line(config_file, "-- after building the site. If false, only build "
                          "the site. Default");
  write_line(config_file, "-- value is true");
  write_line(config_file, "ServerEnabled: " +
                              std::string(yass_config.server_enabled ? "true"
                                                                      : "false"));
  write_line(config_file, "");
  write_line(config_file, "-- Port on which the HTTP server will be running. "
                          "Default value is 8888.");
  write_line(config_file, "-- Valid values are between 1 and 65535");
  write_line(config_file,
             "ServerPort: " + std::to_string(yass_config.server_port));
  write_line(config_file, "");
  write_line(
      config_file,
      "-- If true, when an error during monitoring occurred, stop the server.");
  write_line(config_file,
             "-- Otherwise, show error and wait for changes. Default is true.");
  write_line(config_file,
             "StopServerOnError: " +
                 std::string(yass_config.stop_server_on_error ? "true"
                                                               : "false"));
  write_line(config_file, "");
  write_line(
      config_file,
      "-- Command to execute to open browser with page. Use full path to the web");
  write_line(config_file, "-- browser if it is not available in PATH "
                          "environment variable. Use full");
  write_line(config_file, "-- URL to open as the command parameter. If set "
                          "to 'none' (case sensitive)");
  write_line(config_file,
             "-- browser will not be started. Default value is: none");
  write_line(config_file, "BrowserCommand: " + yass_config.browser_command);
  write_line(config_file, "");
  write_line(config_file, "-- How often in seconds the program should check "
                          "for changes in the site");
  write_line(config_file,
             "-- directory. Default value is 5.0 seconds. Values below 0.5");
  write_line(config_file, "-- are treated as default value.");
  write_line(config_file, "MonitorInterval: " +
                              std::to_string(yass_config.monitor_interval));
  write_line(config_file, "");
  write_line(
      config_file,
      "-- How often in seconds the program should check for changes in the site");
  write_line(config_file, "-- configuration file. Default value is 60.0 "
                          "seconds. Values below 1.0");
  write_line(config_file, "-- are treated as default value.");
  write_line(
      config_file,
      "MonitorConfigInterval: " +
          std::to_string(yass_config.monitor_config_interval));
  write_line(config_file, "");
  write_line(
      config_file,
      "-- Starting sequence for tags in layouts. Default value is: {%");
  write_line(config_file,
             "StartTagSeparator: " + yass_config.start_tag_separator);
  write_line(config_file, "");
  write_line(config_file, "-- Ending sequence for tags in layouts. Default "
                          "value is: %}");
  write_line(config_file,
             "EndTagSeparator: " + yass_config.end_tag_separator);
  write_line(config_file, "");
  write_line(config_file, "-- Starting sequence for markdown comments. "
                          "Default value is: --");
  write_line(config_file,
             "MarkdownComment: " + yass_config.markdown_comment);
  write_line(config_file, "");
  write_line(config_file, "-- List of excluded files from building or "
                          "deleting. Each file or");
  write_line(config_file, "-- directory path should be relative to the site "
                          "directory. All files");
  write_line(config_file, "-- and directories in _layouts, _modules and "
                          "OutputDirectory are excluded");
  write_line(config_file, "-- too. To add exclude file or directory just add "
                          "line Excluded: path");
  write_line(config_file, "-- Examples: Excluded: _cache will exclude "
                          "_cache directory and all files");
  write_line(config_file, "-- in it. Excluded: test.md will exclude test.md "
                          "from the main site");
  write_line(config_file, "-- directory");
  write_line(config_file, "");
  write_line(config_file, "-- Below that line you can add your own tags which "
                          "can be used on any page");
  write_line(config_file, "-- on the site. Tags are in Name: Value scheme.");
  write_line(config_file, "");
  write_line(config_file, "-- Below that line, you can add configuration for "
                          "each page (md files).");
  write_line(config_file, "-- It can be tags for that page and configuration "
                          "options for various");
  write_line(config_file, "-- the program modules. Tags are in Name: Value "
                          "scheme. Composite tags");
  write_line(config_file, "-- (arrays), first must be initialized with Name: "
                          "[] then just add as many");
  write_line(config_file, "-- as you want values to it by Name: Value scheme.");
  write_line(config_file, "-- Configuration for each page must be enclosed in "
                          "[filename] tags,");
  write_line(config_file, "-- where filename is the name of file with or "
                          "without extension.");

  config_file.close();
}

// Helper function to prompt user for input with default value
static std::string ask_user(const std::string& prompt,
                            const std::string& default_value) {
  std::cout << prompt << " [" << default_value << "]: ";
  std::string input;
  std::getline(std::cin, input);

  // If user just pressed enter, use default
  if (input.empty()) {
    return default_value;
  }
  return input;
}

// Helper function to convert string to boolean
static bool to_bool(const std::string& value) {
  std::string lower_value = value;
  for (char& c : lower_value) {
    c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
  }
  return lower_value == "y" || lower_value == "yes" || lower_value.empty();
}

void interactive_site_config() {
  std::cout << "\n=== Site Configuration ===\n\n";

  // Basic site information
  std::cout << "Basic Information:\n";
  yass_config.site_name =
      ask_user("Site name", yass_config.site_name);
  yass_config.site_description =
      ask_user("Site description (for meta tags)", yass_config.site_description);
  yass_config.language = ask_user("Language code (e.g., en, es, fr)",
                                  yass_config.language);
  yass_config.author = ask_user("Author name", yass_config.author);
  yass_config.author_email =
      ask_user("Author email", yass_config.author_email);

  std::cout << "\n";

  // URL and feed settings
  std::cout << "URL and Feed Settings:\n";
  yass_config.base_url = ask_user("Base URL for the site",
                                  yass_config.base_url);

  std::string feed_source = ask_user(
      "Atom feed source (none/tags/filename.md)", yass_config.atom_feed_source);
  yass_config.atom_feed_source = feed_source;

  if (feed_source != "none") {
    std::string feed_amount =
        ask_user("Number of Atom feed entries",
                 std::to_string(yass_config.atom_feed_amount));
    try {
      int amount = std::stoi(feed_amount);
      if (amount > 0) {
        yass_config.atom_feed_amount = amount;
      }
    } catch (...) {
      // Keep default value on parse error
    }
  }

  std::cout << "\n";

  // Basic features
  std::cout << "Basic Features:\n";
  std::string sitemap_answer =
      ask_user("Create sitemap.xml? (yes/no)", "yes");
  yass_config.sitemap_enabled = to_bool(sitemap_answer);

  std::string html_answer =
      ask_user("Convert HTML in markdown? (yes/no)", "yes");
  yass_config.html_enabled = to_bool(html_answer);

  std::cout << "\n";

  // Technical options
  std::cout << "Technical Options:\n";
  std::string server_answer =
      ask_user("Enable web server? (yes/no)", "yes");
  yass_config.server_enabled = to_bool(server_answer);

  if (yass_config.server_enabled) {
    std::string port =
        ask_user("Server port (1-65535)",
                 std::to_string(yass_config.server_port));
    try {
      int port_num = std::stoi(port);
      if (port_num >= 1 && port_num <= 65535) {
        yass_config.server_port = port_num;
      }
    } catch (...) {
      // Keep default value on parse error
    }

    std::string stop_on_error =
        ask_user("Stop server on error? (yes/no)", "yes");
    yass_config.stop_server_on_error = to_bool(stop_on_error);

    yass_config.browser_command =
        ask_user("Browser command (full path or 'none')",
                 yass_config.browser_command);

    std::string monitor_interval = ask_user(
        "Monitor interval in seconds",
        std::to_string(yass_config.monitor_interval));
    try {
      double interval = std::stod(monitor_interval);
      if (interval >= 0.5) {
        yass_config.monitor_interval = interval;
      }
    } catch (...) {
      // Keep default value on parse error
    }

    std::string config_interval = ask_user(
        "Config monitor interval in seconds",
        std::to_string(yass_config.monitor_config_interval));
    try {
      double interval = std::stod(config_interval);
      if (interval >= 1.0) {
        yass_config.monitor_config_interval = interval;
      }
    } catch (...) {
      // Keep default value on parse error
    }
  }

  std::cout << "\n";

  // Compatibility options
  std::cout << "Compatibility Options:\n";
  yass_config.start_tag_separator =
      ask_user("Template start delimiter",
               yass_config.start_tag_separator);
  yass_config.end_tag_separator =
      ask_user("Template end delimiter", yass_config.end_tag_separator);
  yass_config.markdown_comment =
      ask_user("Markdown comment marker", yass_config.markdown_comment);

  std::cout << "\nConfiguration complete!\n\n";
}

// Helper function to trim whitespace from both ends of a string
static std::string trim(const std::string& str) {
  size_t start = 0;
  while (start < str.length() && std::isspace(str[start])) {
    ++start;
  }

  size_t end = str.length();
  while (end > start && std::isspace(str[end - 1])) {
    --end;
  }

  return str.substr(start, end - start);
}

// Helper function to convert string to lowercase
static std::string to_lower_str(const std::string& str) {
  std::string result = str;
  std::transform(result.begin(), result.end(), result.begin(),
                 [](unsigned char c) { return std::tolower(c); });
  return result;
}

// Helper function to split a string by delimiter
static std::vector<std::string> split(const std::string& str, char delimiter) {
  std::vector<std::string> tokens;
  std::string token;
  std::istringstream token_stream(str);

  while (std::getline(token_stream, token, delimiter)) {
    std::string trimmed = trim(token);
    if (!trimmed.empty()) {
      tokens.push_back(trimmed);
    }
  }

  return tokens;
}

bool load_site_config(const std::filesystem::path& directory_name) {
  // Store the site directory globally
  site_directory = directory_name;

  // Reset config to defaults
  yass_config = YassConfig();

  // Clear existing tags and excluded files
  yass_config.tags.clear();
  yass_config.table_tags.clear();
  yass_config.excluded_files.clear();

  // Add default excluded directories
  yass_config.excluded_files.push_back("_layouts");
  yass_config.excluded_files.push_back("_output");
  yass_config.excluded_files.push_back("_modules");
  yass_config.excluded_files.push_back("site.cfg");

  std::filesystem::path config_path = directory_name / "site.cfg";
  std::ifstream config_file(config_path);

  if (!config_file) {
    messages::show_message("Failed to open site.cfg at: " + config_path.string(),
                          messages::MessageType::ERROR);
    return false;
  }

  std::string line;
  int line_number = 0;

  while (std::getline(config_file, line)) {
    ++line_number;
    line = trim(line);

    // Skip empty lines and comments
    if (line.empty() || line[0] == '#' || (line.length() >= 2 && line[0] == '-' && line[1] == '-')) {
      continue;
    }

    // Find the separator (= or :)
    size_t separator_pos = line.find('=');
    if (separator_pos == std::string::npos) {
      separator_pos = line.find(':');
    }

    if (separator_pos == std::string::npos) {
      messages::show_message("Invalid config line " + std::to_string(line_number) + ": " + line,
                            messages::MessageType::ERROR);
      continue;
    }

    std::string field_name = trim(line.substr(0, separator_pos));
    std::string value = trim(line.substr(separator_pos + 1));

    // Parse configuration fields
    if (field_name == "LayoutsDirectory") {
      yass_config.layouts_directory = value;
    } else if (field_name == "OutputDirectory") {
      yass_config.output_directory = value;
    } else if (field_name == "ModulesDirectory") {
      yass_config.modules_directory = value;
    } else if (field_name == "ExcludedFiles") {
      std::vector<std::string> excluded = split(value, ',');
      yass_config.excluded_files.insert(yass_config.excluded_files.end(),
                                       excluded.begin(), excluded.end());
    } else if (field_name == "ServerEnabled") {
      yass_config.server_enabled = (to_lower_str(value) == "true");
    } else if (field_name == "ServerPort") {
      try {
        yass_config.server_port = std::stoi(value);
      } catch (...) {
        messages::show_message("Invalid ServerPort value: " + value,
                              messages::MessageType::ERROR);
      }
    } else if (field_name == "StopServerOnError") {
      yass_config.stop_server_on_error = (to_lower_str(value) == "true");
    } else if (field_name == "BrowserCommand") {
      yass_config.browser_command = value;
    } else if (field_name == "MonitorInterval") {
      try {
        yass_config.monitor_interval = std::stod(value);
      } catch (...) {
        messages::show_message("Invalid MonitorInterval value: " + value,
                              messages::MessageType::ERROR);
      }
    } else if (field_name == "MonitorConfigInterval") {
      try {
        yass_config.monitor_config_interval = std::stod(value);
      } catch (...) {
        messages::show_message("Invalid MonitorConfigInterval value: " + value,
                              messages::MessageType::ERROR);
      }
    } else if (field_name == "BaseURL") {
      yass_config.base_url = value;
      yass_config.tags["BaseURL"] = value;
    } else if (field_name == "SitemapEnabled") {
      yass_config.sitemap_enabled = (to_lower_str(value) == "true");
    } else if (field_name == "HTMLEnabled") {
      yass_config.html_enabled = (to_lower_str(value) == "true");
    } else if (field_name == "AtomFeedSource") {
      yass_config.atom_feed_source = value;
    } else if (field_name == "AtomFeedAmount") {
      try {
        yass_config.atom_feed_amount = std::stoi(value);
      } catch (...) {
        messages::show_message("Invalid AtomFeedAmount value: " + value,
                              messages::MessageType::ERROR);
      }
    } else if (field_name == "Name") {
      yass_config.site_name = value;
      yass_config.tags["Name"] = value;
    } else if (field_name == "Description") {
      yass_config.site_description = value;
      yass_config.tags["Description"] = value;
    } else if (field_name == "Language") {
      yass_config.language = value;
      yass_config.tags["Language"] = value;
    } else if (field_name == "Author") {
      yass_config.author = value;
      yass_config.tags["Author"] = value;
    } else if (field_name == "AuthorEmail") {
      yass_config.author_email = value;
      yass_config.tags["AuthorEmail"] = value;
    } else if (field_name == "StartTagSeparator") {
      yass_config.start_tag_separator = value;
    } else if (field_name == "EndTagSeparator") {
      yass_config.end_tag_separator = value;
    } else if (field_name == "MarkdownComment") {
      yass_config.markdown_comment = value;
    } else {
      // User-defined tag
      // Check if it's initializing a composite tag
      if (value == "[]") {
        yass_config.table_tags[field_name] = std::vector<std::string>();
      } else if (yass_config.table_tags.count(field_name) > 0) {
        // Add to existing composite tag
        yass_config.table_tags[field_name].push_back(value);
      } else {
        // Simple tag
        yass_config.tags[field_name] = value;
      }
    }
  }

  config_file.close();

  // Normalize directory paths to be absolute
  if (!yass_config.layouts_directory.empty() &&
      yass_config.layouts_directory[0] != '/') {
    yass_config.layouts_directory =
      (directory_name / yass_config.layouts_directory).string();
  }

  if (!yass_config.output_directory.empty() &&
      yass_config.output_directory[0] != '/') {
    yass_config.output_directory =
      (directory_name / yass_config.output_directory).string();
  }

  if (!yass_config.modules_directory.empty() &&
      yass_config.modules_directory[0] != '/') {
    yass_config.modules_directory =
      (directory_name / yass_config.modules_directory).string();
  }

  return true;
}

}  // namespace config
