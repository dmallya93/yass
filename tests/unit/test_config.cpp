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

#include <gtest/gtest.h>

#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;

namespace yass::config {
namespace {

class ConfigTest : public ::testing::Test {
protected:
  void SetUp() override {
    // Create a temporary directory for tests
    test_dir = fs::temp_directory_path() / "yass_config_test";
    fs::create_directories(test_dir);

    // Reset global config to defaults before each test
    yass_conf = Parser_Config();
    site_tags.clear();
    global_table_tags.clear();
  }

  void TearDown() override {
    // Clean up temporary directory
    if (fs::exists(test_dir)) {
      fs::remove_all(test_dir);
    }
  }

  // Helper to create a config file with custom content
  void create_custom_config(const std::string& content) {
    std::ofstream config_file(test_dir / "site.cfg");
    config_file << content;
    config_file.close();
  }

  fs::path test_dir;
};

// Test: Create default config file
TEST_F(ConfigTest, CreateDefaultConfig) {
  ASSERT_NO_THROW(create_site_config(test_dir.string()));

  // Verify file exists
  EXPECT_TRUE(fs::exists(test_dir / "site.cfg"));

  // Verify file is not empty
  std::ifstream config_file(test_dir / "site.cfg");
  std::string content((std::istreambuf_iterator<char>(config_file)),
                      std::istreambuf_iterator<char>());
  EXPECT_FALSE(content.empty());
  EXPECT_TRUE(content.find("LayoutsDirectory") != std::string::npos);
  EXPECT_TRUE(content.find("OutputDirectory") != std::string::npos);
}

// Test: Load default config and verify values
TEST_F(ConfigTest, LoadDefaultConfig) {
  // Create default config
  create_site_config(test_dir.string());

  // Load it
  ASSERT_NO_THROW(load_site_config(test_dir.string()));

  // Verify default values are preserved (paths will be normalized)
  EXPECT_TRUE(yass_conf.layouts_directory.find("_layouts") !=
              std::string::npos);
  EXPECT_TRUE(yass_conf.output_directory.find("_output") != std::string::npos);
  EXPECT_TRUE(yass_conf.modules_directory.find("_modules") !=
              std::string::npos);
  EXPECT_EQ(yass_conf.server_enabled, true);
  EXPECT_EQ(yass_conf.server_port, 8888);
  EXPECT_EQ(yass_conf.sitemap_enabled, true);
  EXPECT_EQ(yass_conf.html_enabled, true);
  EXPECT_EQ(yass_conf.stop_server_on_error, false);
  EXPECT_EQ(yass_conf.atom_feed_source, "none");
  EXPECT_EQ(yass_conf.site_name, "New Site");
  EXPECT_EQ(yass_conf.author_name, "John Doe");
  EXPECT_EQ(yass_conf.language, "en");
}

// Test: Parse excluded files list
TEST_F(ConfigTest, ParseExcludedFiles) {
  create_custom_config(R"(
LayoutsDirectory = _layouts
OutputDirectory = _output
ExcludedFiles = .git,.gitignore,tags,test.md
)");

  ASSERT_NO_THROW(load_site_config(test_dir.string()));

  // Verify excluded files were parsed
  EXPECT_GE(yass_conf.excluded_files.size(), 3);

  // Check that some expected files are in the list
  bool found_git = false;
  bool found_tags = false;
  for (const auto& file : yass_conf.excluded_files) {
    if (file == ".git")
      found_git = true;
    if (file == "tags")
      found_tags = true;
  }
  EXPECT_TRUE(found_git);
  EXPECT_TRUE(found_tags);
}

// Test: Parse boolean values
TEST_F(ConfigTest, ParseBooleanValues) {
  create_custom_config(R"(
ServerEnabled = false
SitemapEnabled = true
HTMLEnabled = FALSE
StopServerOnError = True
)");

  ASSERT_NO_THROW(load_site_config(test_dir.string()));

  EXPECT_EQ(yass_conf.server_enabled, false);
  EXPECT_EQ(yass_conf.sitemap_enabled, true);
  EXPECT_EQ(yass_conf.html_enabled, false);
  EXPECT_EQ(yass_conf.stop_server_on_error, true);
}

// Test: Parse numeric values
TEST_F(ConfigTest, ParseNumericValues) {
  create_custom_config(R"(
ServerPort = 3000
AtomFeedAmount = 10
MonitorInterval = 2.5
MonitorConfigInterval = 30.0
)");

  ASSERT_NO_THROW(load_site_config(test_dir.string()));

  EXPECT_EQ(yass_conf.server_port, 3000);
  EXPECT_EQ(yass_conf.atom_feed_amount, 10);
  EXPECT_DOUBLE_EQ(yass_conf.monitor_interval.count(), 2.5);
  EXPECT_DOUBLE_EQ(yass_conf.monitor_config_interval.count(), 30.0);
}

// Test: Validate port range
TEST_F(ConfigTest, ValidatePortRange) {
  create_custom_config(R"(
ServerPort = 70000
)");

  EXPECT_THROW(load_site_config(test_dir.string()), Invalid_Config_Data);
}

// Test: Invalid config without equals sign
TEST_F(ConfigTest, InvalidConfigNoEquals) {
  create_custom_config(R"(
ServerPort 8888
)");

  EXPECT_THROW(load_site_config(test_dir.string()), Invalid_Config_Data);
}

// Test: Comments are ignored
TEST_F(ConfigTest, CommentsIgnored) {
  create_custom_config(R"(
# This is a comment
ServerPort = 9000
# Another comment
Name = Test Site
)");

  ASSERT_NO_THROW(load_site_config(test_dir.string()));
  EXPECT_EQ(yass_conf.server_port, 9000);
  EXPECT_EQ(yass_conf.site_name, "Test Site");
}

// Test: Parse simple custom tags
TEST_F(ConfigTest, ParseSimpleTags) {
  create_custom_config(R"(
Name = My Custom Site
Language = fr
BaseURL = https://example.com
CustomTag = CustomValue
AnotherTag = 12345
)");

  ASSERT_NO_THROW(load_site_config(test_dir.string()));

  // Check site tags
  EXPECT_EQ(site_tags["Name"], "My Custom Site");
  EXPECT_EQ(site_tags["Language"], "fr");
  EXPECT_EQ(site_tags["BaseURL"], "https://example.com");
  EXPECT_EQ(site_tags["CustomTag"], "CustomValue");
  EXPECT_EQ(site_tags["AnotherTag"], "12345");
}

// Test: Parse composite/table tags
TEST_F(ConfigTest, ParseCompositeTags) {
  create_custom_config(R"(
MenuItems = []
MenuItems = Home
MenuItems = About
MenuItems = Contact
Categories = []
Categories = Tech
Categories = News
)");

  ASSERT_NO_THROW(load_site_config(test_dir.string()));

  // Check table tags
  ASSERT_EQ(global_table_tags.count("MenuItems"), 1);
  ASSERT_EQ(global_table_tags.count("Categories"), 1);

  const auto& menu_items = global_table_tags["MenuItems"];
  ASSERT_EQ(menu_items.size(), 3);
  EXPECT_EQ(menu_items[0], "Home");
  EXPECT_EQ(menu_items[1], "About");
  EXPECT_EQ(menu_items[2], "Contact");

  const auto& categories = global_table_tags["Categories"];
  ASSERT_EQ(categories.size(), 2);
  EXPECT_EQ(categories[0], "Tech");
  EXPECT_EQ(categories[1], "News");
}

// Test: Browser command with %s substitution
TEST_F(ConfigTest, BrowserCommandSubstitution) {
  create_custom_config(R"(
ServerPort = 5000
BrowserCommand = firefox %s
)");

  ASSERT_NO_THROW(load_site_config(test_dir.string()));

  EXPECT_EQ(yass_conf.browser_command, "firefox http://localhost:5000");
}

// Test: AtomFeedSource conversion
TEST_F(ConfigTest, AtomFeedSourceConversion) {
  create_custom_config(R"(
AtomFeedSource = feed.md
)");

  ASSERT_NO_THROW(load_site_config(test_dir.string()));

  EXPECT_EQ(yass_conf.atom_feed_source, "feed.html");
}

// Test: AtomFeedSource none and tags
TEST_F(ConfigTest, AtomFeedSourceSpecialValues) {
  // Test "none"
  create_custom_config("AtomFeedSource = none\n");
  ASSERT_NO_THROW(load_site_config(test_dir.string()));
  EXPECT_EQ(yass_conf.atom_feed_source, "none");

  // Reset and test "tags"
  yass_conf = Parser_Config();
  site_tags.clear();
  create_custom_config("AtomFeedSource = tags\n");
  ASSERT_NO_THROW(load_site_config(test_dir.string()));
  EXPECT_EQ(yass_conf.atom_feed_source, "tags");
}

// Test: Create and reload roundtrip
TEST_F(ConfigTest, CreateAndReloadRoundtrip) {
  // Create default config
  ASSERT_NO_THROW(create_site_config(test_dir.string()));

  // Load it
  ASSERT_NO_THROW(load_site_config(test_dir.string()));

  // Verify a few key values
  EXPECT_TRUE(yass_conf.layouts_directory.find("_layouts") !=
              std::string::npos);
  EXPECT_TRUE(yass_conf.output_directory.find("_output") != std::string::npos);
  EXPECT_EQ(yass_conf.server_port, 8888);
}

// Test: Empty directory name throws
TEST_F(ConfigTest, EmptyDirectoryThrows) {
  EXPECT_THROW(create_site_config(""), std::invalid_argument);
  EXPECT_THROW(load_site_config(""), std::invalid_argument);
}

// Test: Non-existent config file throws
TEST_F(ConfigTest, NonExistentConfigThrows) {
  fs::path non_existent = test_dir / "does_not_exist";
  EXPECT_THROW(load_site_config(non_existent.string()), std::runtime_error);
}

// Test: Whitespace trimming
TEST_F(ConfigTest, WhitespaceTrimming) {
  create_custom_config(R"(
  ServerPort  =  9999
  Name  =   Test Site
)");

  ASSERT_NO_THROW(load_site_config(test_dir.string()));
  EXPECT_EQ(yass_conf.server_port, 9999);
  EXPECT_EQ(yass_conf.site_name, "Test Site");
}

}  // anonymous namespace
}  // namespace yass::config
