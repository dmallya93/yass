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

#include "config/config.hpp"
#include <algorithm>
#include <cctype>
#include <fstream>
#include <iostream>
#include <sstream>

namespace config {

// Initialize global variables
ParserConfig yass_conf;
std::unordered_map<std::string, std::string> site_tags;
std::filesystem::path site_directory;
std::unordered_map<std::string, std::vector<std::string>> global_table_tags;

namespace {

// Helper function to trim whitespace from both ends of a string
std::string trim(const std::string& str) {
  const auto START = std::find_if_not(
      str.begin(), str.end(), [](unsigned char c) { return std::isspace(c); });
  const auto END =
      std::find_if_not(str.rbegin(), str.rend(),
                       [](unsigned char c) { return std::isspace(c); })
          .base();
  return (START < END) ? std::string(START, END) : std::string();
}

// Helper function to convert string to lowercase
std::string to_lower(const std::string& str) {
  std::string result = str;
  std::transform(result.begin(), result.end(), result.begin(),
                 [](unsigned char c) { return std::tolower(c); });
  return result;
}

// Helper function to normalize directory path
std::filesystem::path normalize_dir(const std::filesystem::path& dir_path,
                                    const std::filesystem::path& base_dir) {
  if (dir_path.is_absolute()) {
    return dir_path;
  }
  return base_dir / dir_path;
}

// Helper function to split string by delimiter
std::vector<std::string> split(const std::string& str, char delimiter) {
  std::vector<std::string> tokens;
  std::stringstream ss(str);
  std::string token;
  while (std::getline(ss, token, delimiter)) {
    std::string trimmed = trim(token);
    if (!trimmed.empty()) {
      tokens.push_back(trimmed);
    }
  }
  return tokens;
}

} // anonymous namespace

void create_site_config(const std::filesystem::path& directory_name) {
  std::filesystem::path config_path = directory_name / "site.cfg";
  std::ofstream config_file(config_path);

  if (!config_file) {
    throw InvalidConfigData("Cannot create config file: " +
                            config_path.string());
  }

  config_file << "# Directory in which will be placed HTML files with site "
                 "layout\n";
  config_file << "# (templates). May be absolute or relative to project "
                 "directory.\n";
  config_file << "LayoutsDirectory = _layouts\n\n";

  config_file << "# Directory in which will be placed generated site. May be "
                 "absolute\n";
  config_file << "# or relative to project directory.\n";
  config_file << "OutputDirectory = _output\n\n";

  config_file << "# Directory in which will be placed program modules used to "
                 "generate\n";
  config_file << "# the site. May be absolute or relative to project "
                 "directory.\n";
  config_file << "ModulesDirectory = _modules\n\n";

  config_file << "# List of excluded files and directories from list of "
                 "sources used to\n";
  config_file << "# generating the site. All paths must be relative to the "
                 "project\n";
  config_file << "# directory. If you exclude directory, it whole content "
                 "will be\n";
  config_file << "# excluded too. Layouts, modules and output directories are "
                 "excluded\n";
  config_file << "# by default.\n";
  config_file << "ExcludedFiles = .git,.gitignore,tags\n\n";

  config_file << "# The name of the site which will be created. If you have "
                 "enabled\n";
  config_file << "# creating Atom feed then it is needed. Otherwise, you can "
                 "use it as a\n";
  config_file << "# normal template tag.\n";
  config_file << "Name = " << yass_conf.site_name << "\n\n";

  config_file << "# The description of the site which will be created. Must "
                 "be in one line,\n";
  config_file << "# no new line allowed. It is used to set meta tag "
                 "description (which is\n";
  config_file << "# showed in search engines results) but only when pages "
                 "don't set it.\n";
  config_file << "# Optional setting.\n";
  config_file << "Description = " << yass_conf.description << "\n\n";

  config_file << "# The ISO 639-1 language code in which the site will be "
                 "created.\n";
  config_file << "Language = " << yass_conf.language << "\n\n";

  config_file << "# Name of author of the site. If you have enable creating "
                 "Atom feed,\n";
  config_file << "# then it is needed. Otherwise, you can use it as a normal "
                 "template tag.\n";
  config_file << "# It is also used in setting meta tag author for all "
                 "pages.\n";
  config_file << "Author = " << yass_conf.author_name << "\n\n";

  config_file << "# Email address of author of the site. If you have enable "
                 "creating Atom\n";
  config_file << "# feed, then it is needed. Otherwise, you can use it as a "
                 "normal\n";
  config_file << "# template tag.\n";
  config_file << "AuthorEmail = " << yass_conf.author_email << "\n\n";

  config_file << "# Base URL of the site. It is needed mostly for creating "
                 "sitemap and\n";
  config_file << "# Atom feed, but you can use it as a normal the site tag. "
                 "If your site\n";
  config_file << "# will be available at https://mysite.com/blog then this "
                 "will be your\n";
  config_file << "# BaseURL.\n";
  config_file << "BaseURL = " << yass_conf.base_url << "\n\n";

  config_file << "# Source which will be used for creating Atom feed of the "
                 "site.\n";
  config_file << "# Possible values are: none: don't create atom feed, tags: "
                 "create\n";
  config_file << "# Atom entries from proper tags in .md files, [filename]: "
                 "the path\n";
  config_file << "# (related to the project directory path) to markdown file "
                 "which will\n";
  config_file << "# be used as a source of atom feed (must have proper tags "
                 "set inside).\n";
  config_file << "AtomFeedSource = " << yass_conf.atom_feed_source << "\n\n";

  config_file << "# Number of entries in the Atom feed of the site. Try not "
                 "set it too\n";
  config_file << "# high, recommended values are between 10 and 50.\n";
  config_file << "AtomFeedAmount = " << yass_conf.atom_feed_amount << "\n\n";

  config_file << "# Should the program create sitemap when creating the site. "
                 "Possible\n";
  config_file << "# values are true or false (case-insensitive).\n";
  config_file << "SitemapEnabled = "
              << (yass_conf.sitemap_enabled ? "true" : "false") << "\n\n";

  config_file << "# Should program convert HTML in markdown documents to "
                 "actual HTML.\n";
  config_file << "# Possible values are true or false (case-insensitive).\n";
  config_file << "HTMLEnabled = " << (yass_conf.html_enabled ? "true" : "false")
              << "\n\n";

  config_file << "# Should the program start web server when monitoring for "
                 "changes in\n";
  config_file << "# site. Possible values are true or false "
                 "(case-insensitive).\n";
  config_file << "ServerEnabled = "
              << (yass_conf.server_enabled ? "true" : "false") << "\n\n";

  config_file << "# Port on which web server will be listen if enabled. "
                 "Possible values\n";
  config_file << "# are from 1 to 65535. Please remember, that ports below "
                 "1025 require\n";
  config_file << "# root privileges to work.\n";
  config_file << "ServerPort = " << yass_conf.server_port << "\n\n";

  config_file << "# Should web server and whole monitoring of the site changes "
                 "stop if\n";
  config_file << "# encounter any error during the site creation.  Possible "
                 "values are\n";
  config_file << "# true or false (case-insensitive).\n";
  config_file << "StopServerOnError = "
              << (yass_conf.stop_server_on_error ? "true" : "false") << "\n\n";

  config_file << "# Full path to the command which will be used to start the "
                 "web browser\n";
  config_file << "# with index.html page of the site. String \"%s\" (without "
                 "quotes) will\n";
  config_file << "# be replaced by server URL. If this setting is \"none\", "
                 "the web\n";
  config_file << "# browser will be not started, same as when the web server "
                 "is disabled.\n";
  config_file << "BrowserCommand = " << yass_conf.browser_command << "\n\n";

  config_file << "# How often (in seconds) the program should monitor site for "
                 "changes\n";
  config_file << "# and regenerate it if needed. Can be any positive number, "
                 "but you\n";
  config_file << "# probably don't want to set it to check every few thousands "
                 "years :)\n";
  config_file << "MonitorInterval = " << yass_conf.monitor_interval.count()
              << "\n\n";

  config_file << "# How often (in seconds) the program should monitor site "
                 "configuration\n";
  config_file << "# for changes and reconfigure it if needed. Can be any "
                 "positive number.\n";
  config_file << "MonitorConfigInterval = "
              << yass_conf.monitor_config_interval.count() << "\n\n";

  config_file << "# String used to mark start of the templates tags, used in "
                 "templates\n";
  config_file << "# files. You may want to change it, if you want to use "
                 "templates from\n";
  config_file << "# other static site generator.\n";
  config_file << "StartTagSeparator = " << yass_conf.start_tag_separator
              << "\n\n";

  config_file << "# String used to mark end of the templates tags, used in "
                 "templates\n";
  config_file << "# files. You may want to change it, if you want to use "
                 "templates from\n";
  config_file << "# other static site generator.\n";
  config_file << "EndTagSeparator = " << yass_conf.end_tag_separator << "\n\n";

  config_file << "# String used to mark comments in markdown files which will "
                 "be parsed.\n";
  config_file << "MarkdownComment = " << yass_conf.markdown_comment << "\n\n";

  config_file << "# Site tags, optional. Tags can be 4 types: strings, "
                 "boolean, numeric\n";
  config_file << "# or composite.\n";
  config_file << "# First 3 types of tags are in Name = Value scheme. For "
                 "strings, it can\n";
  config_file << "# be any alphanumeric value without new line sign. For "
                 "boolean it must\n";
  config_file << "# be \"true\" or \"false\", for numeric any number. Program "
                 "will detect\n";
  config_file << "# self which type of tag is and properly set it. It always "
                 "falls back\n";
  config_file << "# to string value.\n";
  config_file << "# Composite tags first must be initialized with Name = [] "
                 "then just\n";
  config_file << "# add as many as you want values to it by Name = Value "
                 "scheme.\n";
  config_file << "# For more information about site.cfg file please check "
                 "program\n";
  config_file << "# documentation.\n";

  config_file.close();
}

void load_site_config(const std::filesystem::path& directory_name) {
  site_tags.clear();
  global_table_tags.clear();

  std::filesystem::path config_path = directory_name / "site.cfg";
  std::ifstream config_file(config_path);

  if (!config_file) {
    throw InvalidConfigData("Cannot open config file: " +
                            config_path.string());
  }

  std::string line;
  int line_number = 0;

  while (std::getline(config_file, line)) {
    ++line_number;

    // Skip empty lines and comments
    std::string trimmed_line = trim(line);
    if (trimmed_line.empty() || trimmed_line[0] == '#') {
      continue;
    }

    // Find the '=' separator
    size_t equal_pos = trimmed_line.find('=');
    if (equal_pos == std::string::npos) {
      throw InvalidConfigData("Line " + std::to_string(line_number) +
                              ": Missing '=' in: " + trimmed_line);
    }

    std::string field_name = trim(trimmed_line.substr(0, equal_pos));
    std::string value = trim(trimmed_line.substr(equal_pos + 1));

    try {
      if (field_name == "LayoutsDirectory") {
        yass_conf.layouts_directory = value;
      } else if (field_name == "OutputDirectory") {
        yass_conf.output_directory = value;
      } else if (field_name == "ModulesDirectory") {
        yass_conf.modules_directory = value;
      } else if (field_name == "ExcludedFiles") {
        yass_conf.excluded_files = split(value, ',');
      } else if (field_name == "ServerEnabled") {
        yass_conf.server_enabled = (to_lower(value) == "true");
      } else if (field_name == "ServerPort") {
        yass_conf.server_port = std::stoi(value);
        if (yass_conf.server_port < 1 || yass_conf.server_port > 65535) {
          throw InvalidConfigData("ServerPort must be between 1 and 65535");
        }
      } else if (field_name == "StopServerOnError") {
        yass_conf.stop_server_on_error = (to_lower(value) == "true");
      } else if (field_name == "BrowserCommand") {
        // Replace %s with actual server URL
        size_t pos = value.find("%s");
        if (pos != std::string::npos) {
          value.replace(pos, 2, "http://localhost:" +
                                     std::to_string(yass_conf.server_port));
        }
        yass_conf.browser_command = value;
      } else if (field_name == "MonitorInterval") {
        yass_conf.monitor_interval =
            std::chrono::duration<double>(std::stod(value));
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
          // Convert .md to .html for file-based feed source
          if (value.size() > 3 && value.substr(value.size() - 3) == ".md") {
            yass_conf.atom_feed_source = value.substr(0, value.size() - 3) + ".html";
          } else {
            yass_conf.atom_feed_source = value;
          }
        }
      } else if (field_name == "AtomFeedAmount") {
        yass_conf.atom_feed_amount = std::stoi(value);
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
      } else if (global_table_tags.find(field_name) !=
                 global_table_tags.end()) {
        // Append to composite tag
        global_table_tags[field_name].push_back(value);
      } else {
        // Generic site tag
        site_tags[field_name] = value;
      }
    } catch (const std::exception& e) {
      throw InvalidConfigData("Line " + std::to_string(line_number) + ": " +
                              e.what() + " in: " + trimmed_line);
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
  yass_conf.excluded_files.emplace_back(
      yass_conf.layouts_directory.filename().string());
  yass_conf.excluded_files.emplace_back(
      yass_conf.output_directory.filename().string());
  yass_conf.excluded_files.emplace_back(
      yass_conf.modules_directory.filename().string());

  site_directory = directory_name;
}

void interactive_site_config() {
  std::string input;

  std::cout << "Now we ask you some questions about your new site.\n";
  std::cout << "You can always change it later by modifying the site "
               "configuration\n";
  std::cout << "file. If you just press Enter as a answer, default value will "
               "be\n";
  std::cout << "used.\n\n";

  auto ask_user = [](const std::string& default_value) -> std::string {
    std::cout << "(Default - " << default_value << ") > ";
    std::string answer;
    std::getline(std::cin, answer);
    return answer.empty() ? default_value : answer;
  };

  std::cout << "Please enter the name of the new site\n\n";
  yass_conf.site_name = ask_user("New Site");
  std::cout << "\n";

  std::cout << "Please enter the description of the new site.\n";
  std::cout << "It is used to create meta tag for the website (which is "
               "showed\n";
  std::cout << "in search engines results) but only if pages don't set own.\n";
  std::cout << "Must be set in one line, no new line allowed.\n\n";
  yass_conf.description = ask_user("My new site");
  std::cout << "\n";

  std::cout << "Please enter language code in which the new site will be "
               "written\n\n";
  yass_conf.language = ask_user("en");
  std::cout << "\n";

  std::cout << "Please enter the author of the new site\n\n";
  yass_conf.author_name = ask_user("John Doe");
  std::cout << "\n";

  std::cout << "Please enter the contact email for the new site\n\n";
  yass_conf.author_email = ask_user("johndoe@example.com");
  std::cout << "\n";

  std::cout << "Please enter base URL of the new site\n\n";
  yass_conf.base_url = ask_user("http://localhost:8888");
  std::cout << "\n";

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
  std::cout << "\n";

  if (yass_conf.atom_feed_source != "none") {
    std::cout << "How much maximum entries should be in the Atom feed?\n";
    std::cout << "Recommended valuese are between 10 and 50.\n\n";
    yass_conf.atom_feed_amount = std::stoi(ask_user("25"));
    std::cout << "\n";
  }

  std::cout << "Do you want to create sitemap file for the new site?\n\n";
  std::string sitemap_answer = ask_user("yes");
  yass_conf.sitemap_enabled =
      (sitemap_answer == "yes" || sitemap_answer == "y" || sitemap_answer.empty());
  std::cout << "\n";

  std::cout << "Do you want to HTML embedded in your markdown to be "
               "converted?\n\n";
  std::string html_answer = ask_user("yes");
  yass_conf.html_enabled =
      (html_answer == "yes" || html_answer == "y" || html_answer.empty());
  std::cout << "\n";

  std::cout << "Do you want to set more technical options (like configuring\n";
  std::cout << "build-in web server)?\n\n";
  std::string tech_answer = ask_user("no");
  std::cout << "\n";

  if (tech_answer == "yes" || tech_answer == "y") {
    std::cout << "Should the program start web server when monitoring for\n";
    std::cout << "changes in the site?\n\n";
    std::string server_answer = ask_user("yes");
    yass_conf.server_enabled =
        (server_answer == "yes" || server_answer == "y" || server_answer.empty());
    std::cout << "\n";

    std::cout << "On which port should the web server listening?\n";
    std::cout << "Possible values are from 1 to 65535. Ports below 1025 "
                 "require\n";
    std::cout << "root privileges.\n\n";
    yass_conf.server_port = std::stoi(ask_user("8888"));
    std::cout << "\n";

    std::cout << "Should whole monitoring option stop if encounter any error\n";
    std::cout << "during the site creation?\n\n";
    std::string stop_answer = ask_user("no");
    yass_conf.stop_server_on_error =
        (stop_answer == "yes" || stop_answer == "y");
    std::cout << "\n";

    std::cout << "Full path to the web broser which will be started when the\n";
    std::cout << "program starts in server mode.\n\n";
    yass_conf.browser_command = ask_user("none");
    std::cout << "\n";

    std::cout << "How often, in seconds, the program should check for changes\n";
    std::cout << "in the site files?\n\n";
    yass_conf.monitor_interval =
        std::chrono::duration<double>(std::stod(ask_user("5.0")));
    std::cout << "\n";

    std::cout << "How often, in seconds, the program should check for changes\n";
    std::cout << "in the site configuration file?\n\n";
    yass_conf.monitor_config_interval =
        std::chrono::duration<double>(std::stod(ask_user("60.0")));
    std::cout << "\n";
  }

  std::cout << "Do you want to set options related to compatybility with "
               "other\n";
  std::cout << "static sites generators?\n\n";
  std::string compat_answer = ask_user("no");
  std::cout << "\n";

  if (compat_answer == "yes" || compat_answer == "y") {
    std::cout << "What mark should be used as a start for template tag?\n\n";
    yass_conf.start_tag_separator = ask_user("{%");
    std::cout << "\n";

    std::cout << "What mark should be used as an end for template tag?\n\n";
    yass_conf.end_tag_separator = ask_user("%}");
    std::cout << "\n";

    std::cout << "What mark should be used as a start for the comment line "
                 "in\n";
    std::cout << "Markdown files?\n\n";
    yass_conf.markdown_comment = ask_user("--");
  }
}

} // namespace config
