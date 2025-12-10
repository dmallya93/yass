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

#include <gtest/gtest.h>

#include <filesystem>
#include <fstream>
#include <string>

namespace yass {
namespace {

class ConfigTest : public ::testing::Test {
 protected:
  void SetUp() override {
    // Create temporary test directory
    test_dir_ = std::filesystem::temp_directory_path() / "yass_test_config";
    std::filesystem::create_directories(test_dir_);

    // Reset global configuration to defaults before each test
    yass_conf = DEFAULT_PARSER_CONFIGURATION;
    site_tags.clear();
    global_table_tags.clear();
    site_directory.clear();
  }

  void TearDown() override {
    // Clean up test directory
    if (std::filesystem::exists(test_dir_)) {
      std::filesystem::remove_all(test_dir_);
    }
  }

  std::filesystem::path test_dir_;  // NOLINT(cppcoreguidelines-non-private-member-variables-in-classes)
};

// Test create_site_config function
TEST_F(ConfigTest, CreateSiteConfig) {
  ASSERT_NO_THROW(create_site_config(test_dir_.string()));

  // Verify config file was created
  std::filesystem::path config_path = test_dir_ / "site.cfg";
  EXPECT_TRUE(std::filesystem::exists(config_path));

  // Read and verify content
  std::ifstream config_file(config_path);
  ASSERT_TRUE(config_file.is_open());

  std::string content((std::istreambuf_iterator<char>(config_file)),
                      std::istreambuf_iterator<char>());

  // Check for key configuration options
  EXPECT_NE(content.find("LayoutsDirectory = _layouts"), std::string::npos);
  EXPECT_NE(content.find("OutputDirectory = _output"), std::string::npos);
  EXPECT_NE(content.find("ModulesDirectory = _modules"), std::string::npos);
  EXPECT_NE(content.find("Name = New Site"), std::string::npos);
  EXPECT_NE(content.find("ServerPort = 8888"), std::string::npos);
}

// Test create_site_config with empty directory name
TEST_F(ConfigTest, CreateSiteConfigEmptyDirectory) {
  EXPECT_THROW(create_site_config(""), std::invalid_argument);
}

// Test load_site_config function with valid config
TEST_F(ConfigTest, LoadSiteConfig) {
  // Create a test config file
  create_site_config(test_dir_.string());

  // Modify some values in the config
  std::filesystem::path config_path = test_dir_ / "site.cfg";
  std::ofstream config_file(config_path, std::ios::app);
  config_file << "TestTag = TestValue\n";
  config_file.close();

  // Load the config
  ASSERT_NO_THROW(load_site_config(test_dir_.string()));

  // Verify loaded values
  EXPECT_EQ(yass_conf.site_name, "New Site");
  EXPECT_EQ(yass_conf.server_port, 8888U);
  EXPECT_EQ(yass_conf.sitemap_enabled, true);
  EXPECT_EQ(yass_conf.html_enabled, true);
  EXPECT_EQ(yass_conf.server_enabled, true);
  EXPECT_EQ(yass_conf.atom_feed_source, "none");

  // Verify site tags
  EXPECT_NE(site_tags.find("TestTag"), site_tags.end());
  EXPECT_EQ(site_tags["TestTag"], "TestValue");

  // Verify site directory was set
  EXPECT_EQ(site_directory, test_dir_.string());
}

// Test load_site_config with custom values
TEST_F(ConfigTest, LoadSiteConfigCustomValues) {
  // Create a custom config file
  std::filesystem::path config_path = test_dir_ / "site.cfg";
  std::ofstream config_file(config_path);
  config_file << "Name = My Custom Site\n";
  config_file << "Author = Jane Doe\n";
  config_file << "AuthorEmail = jane@example.com\n";
  config_file << "Language = fr\n";
  config_file << "ServerPort = 9000\n";
  config_file << "ServerEnabled = false\n";
  config_file << "SitemapEnabled = false\n";
  config_file << "HTMLEnabled = false\n";
  config_file << "MonitorInterval = 10.0\n";
  config_file << "MonitorConfigInterval = 120.0\n";
  config_file << "AtomFeedAmount = 50\n";
  config_file.close();

  load_site_config(test_dir_.string());

  EXPECT_EQ(yass_conf.site_name, "My Custom Site");
  EXPECT_EQ(yass_conf.author_name, "Jane Doe");
  EXPECT_EQ(yass_conf.author_email, "jane@example.com");
  EXPECT_EQ(yass_conf.language, "fr");
  EXPECT_EQ(yass_conf.server_port, 9000U);
  EXPECT_EQ(yass_conf.server_enabled, false);
  EXPECT_EQ(yass_conf.sitemap_enabled, false);
  EXPECT_EQ(yass_conf.html_enabled, false);
  EXPECT_EQ(yass_conf.monitor_interval.count(), 10.0);
  EXPECT_EQ(yass_conf.monitor_config_interval.count(), 120.0);
  EXPECT_EQ(yass_conf.atom_feed_amount, 50U);
}

// Test load_site_config with excluded files
TEST_F(ConfigTest, LoadSiteConfigExcludedFiles) {
  std::filesystem::path config_path = test_dir_ / "site.cfg";
  std::ofstream config_file(config_path);
  config_file << "Name = Test\n";
  config_file << "ExcludedFiles = .git,.gitignore,tags\n";
  config_file.close();

  load_site_config(test_dir_.string());

  // Check that excluded files were loaded
  EXPECT_GE(yass_conf.excluded_files.size(), 3U);
  bool found_git = false;
  bool found_gitignore = false;
  bool found_tags = false;
  for (const auto& file : yass_conf.excluded_files) {
    if (file == ".git") {
      found_git = true;
    }
    if (file == ".gitignore") {
      found_gitignore = true;
    }
    if (file == "tags") {
      found_tags = true;
    }
  }
  EXPECT_TRUE(found_git);
  EXPECT_TRUE(found_gitignore);
  EXPECT_TRUE(found_tags);
}

// Test load_site_config with composite tags
TEST_F(ConfigTest, LoadSiteConfigCompositeTags) {
  std::filesystem::path config_path = test_dir_ / "site.cfg";
  std::ofstream config_file(config_path);
  config_file << "Name = Test\n";
  config_file << "MyList = []\n";
  config_file << "MyList = Item1\n";
  config_file << "MyList = Item2\n";
  config_file << "MyList = Item3\n";
  config_file.close();

  load_site_config(test_dir_.string());

  // Verify composite tag
  ASSERT_NE(global_table_tags.find("MyList"), global_table_tags.end());
  EXPECT_EQ(global_table_tags["MyList"].size(), 3U);
  EXPECT_EQ(global_table_tags["MyList"][0], "Item1");
  EXPECT_EQ(global_table_tags["MyList"][1], "Item2");
  EXPECT_EQ(global_table_tags["MyList"][2], "Item3");
}

// Test load_site_config with comments
TEST_F(ConfigTest, LoadSiteConfigWithComments) {
  std::filesystem::path config_path = test_dir_ / "site.cfg";
  std::ofstream config_file(config_path);
  config_file << "# This is a comment\n";
  config_file << "Name = Test Site\n";
  config_file << "# Another comment\n";
  config_file << "\n";  // Empty line
  config_file << "Author = Test Author\n";
  config_file.close();

  ASSERT_NO_THROW(load_site_config(test_dir_.string()));
  EXPECT_EQ(yass_conf.site_name, "Test Site");
  EXPECT_EQ(yass_conf.author_name, "Test Author");
}

// Test load_site_config with invalid port
TEST_F(ConfigTest, LoadSiteConfigInvalidPort) {
  std::filesystem::path config_path = test_dir_ / "site.cfg";
  std::ofstream config_file(config_path);
  config_file << "Name = Test\n";
  config_file << "ServerPort = 70000\n";  // Invalid port > 65535
  config_file.close();

  EXPECT_THROW(load_site_config(test_dir_.string()), InvalidConfigData);
}

// Test load_site_config with missing equals sign
TEST_F(ConfigTest, LoadSiteConfigMissingEquals) {
  std::filesystem::path config_path = test_dir_ / "site.cfg";
  std::ofstream config_file(config_path);
  config_file << "Name Test\n";  // Missing =
  config_file.close();

  EXPECT_THROW(load_site_config(test_dir_.string()), InvalidConfigData);
}

// Test load_site_config with empty directory name
TEST_F(ConfigTest, LoadSiteConfigEmptyDirectory) {
  EXPECT_THROW(load_site_config(""), std::invalid_argument);
}

// Test load_site_config with non-existent file
TEST_F(ConfigTest, LoadSiteConfigNonExistentFile) {
  EXPECT_THROW(load_site_config(test_dir_.string()), std::runtime_error);
}

// Test browser command with %s placeholder
TEST_F(ConfigTest, LoadSiteConfigBrowserCommand) {
  std::filesystem::path config_path = test_dir_ / "site.cfg";
  std::ofstream config_file(config_path);
  config_file << "Name = Test\n";
  config_file << "ServerPort = 8080\n";
  config_file << "BrowserCommand = firefox %s\n";
  config_file.close();

  load_site_config(test_dir_.string());

  EXPECT_EQ(yass_conf.browser_command, "firefox http://localhost:8080");
}

// Test atom feed source with markdown file
TEST_F(ConfigTest, LoadSiteConfigAtomFeedSourceMarkdown) {
  std::filesystem::path config_path = test_dir_ / "site.cfg";
  std::ofstream config_file(config_path);
  config_file << "Name = Test\n";
  config_file << "AtomFeedSource = blog/feed.md\n";
  config_file.close();

  load_site_config(test_dir_.string());

  EXPECT_EQ(yass_conf.atom_feed_source, "blog/feed.html");
}

// Test default parser configuration
TEST_F(ConfigTest, DefaultParserConfiguration) {
  EXPECT_EQ(DEFAULT_PARSER_CONFIGURATION.output_directory, "_output");
  EXPECT_EQ(DEFAULT_PARSER_CONFIGURATION.layouts_directory, "_layouts");
  EXPECT_EQ(DEFAULT_PARSER_CONFIGURATION.modules_directory, "_modules");
  EXPECT_EQ(DEFAULT_PARSER_CONFIGURATION.server_enabled, true);
  EXPECT_EQ(DEFAULT_PARSER_CONFIGURATION.server_port, 8888U);
  EXPECT_EQ(DEFAULT_PARSER_CONFIGURATION.sitemap_enabled, true);
  EXPECT_EQ(DEFAULT_PARSER_CONFIGURATION.html_enabled, true);
  EXPECT_EQ(DEFAULT_PARSER_CONFIGURATION.atom_feed_source, "none");
  EXPECT_EQ(DEFAULT_PARSER_CONFIGURATION.site_name, "New Site");
  EXPECT_EQ(DEFAULT_PARSER_CONFIGURATION.atom_feed_amount, 25U);
  EXPECT_EQ(DEFAULT_PARSER_CONFIGURATION.markdown_comment, "--");
  EXPECT_EQ(DEFAULT_PARSER_CONFIGURATION.stop_server_on_error, false);
  EXPECT_EQ(DEFAULT_PARSER_CONFIGURATION.browser_command, "none");
  EXPECT_EQ(DEFAULT_PARSER_CONFIGURATION.monitor_interval.count(), 5.0);
  EXPECT_EQ(DEFAULT_PARSER_CONFIGURATION.monitor_config_interval.count(), 60.0);
  EXPECT_EQ(DEFAULT_PARSER_CONFIGURATION.author_name, "John Doe");
  EXPECT_EQ(DEFAULT_PARSER_CONFIGURATION.author_email, "johndoe@example.com");
  EXPECT_EQ(DEFAULT_PARSER_CONFIGURATION.language, "en");
  EXPECT_EQ(DEFAULT_PARSER_CONFIGURATION.start_tag_separator, "{%");
  EXPECT_EQ(DEFAULT_PARSER_CONFIGURATION.end_tag_separator, "%}");
  EXPECT_EQ(DEFAULT_PARSER_CONFIGURATION.description, "My new site");
}

}  // namespace
}  // namespace yass
