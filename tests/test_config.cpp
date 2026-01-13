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
#include <fstream>
#include <gtest/gtest.h>

class ConfigTest : public ::testing::Test {
private:
  std::filesystem::path test_dir_;

protected:
  void SetUp() override {
    // Reset global config to defaults
    config::yass_conf = config::ParserConfig();
    config::site_tags.clear();
    config::global_table_tags.clear();
    config::site_directory.clear();

    // Create test directory
    test_dir_ = std::filesystem::temp_directory_path() / "yass_test_config";
    std::filesystem::create_directories(test_dir_);
  }

  void TearDown() override {
    // Clean up test directory
    if (std::filesystem::exists(test_dir_)) {
      std::filesystem::remove_all(test_dir_);
    }
  }

  [[nodiscard]] const std::filesystem::path& test_dir() const { return test_dir_; }
};

TEST_F(ConfigTest, CreateSiteConfig) {
  config::create_site_config(test_dir());

  std::filesystem::path config_path = test_dir() / "site.cfg";
  ASSERT_TRUE(std::filesystem::exists(config_path))
      << "Config file should be created";

  // Check that file is not empty
  std::ifstream file(config_path);
  std::string content((std::istreambuf_iterator<char>(file)),
                      std::istreambuf_iterator<char>());
  ASSERT_FALSE(content.empty()) << "Config file should not be empty";

  // Check for some expected content
  EXPECT_NE(content.find("LayoutsDirectory"), std::string::npos);
  EXPECT_NE(content.find("OutputDirectory"), std::string::npos);
  EXPECT_NE(content.find("ModulesDirectory"), std::string::npos);
}

TEST_F(ConfigTest, LoadSiteConfigBasic) {
  // Create a basic config file
  std::filesystem::path config_path = test_dir() / "site.cfg";
  std::ofstream config_file(config_path);
  config_file << "Name = Test Site\n";
  config_file << "Language = en\n";
  config_file << "Author = Test Author\n";
  config_file << "BaseURL = http://test.com\n";
  config_file.close();

  config::load_site_config(test_dir());

  EXPECT_EQ(config::yass_conf.site_name, "Test Site");
  EXPECT_EQ(config::yass_conf.language, "en");
  EXPECT_EQ(config::yass_conf.author_name, "Test Author");
  EXPECT_EQ(config::yass_conf.base_url, "http://test.com");
  EXPECT_EQ(config::site_directory, test_dir());
}

TEST_F(ConfigTest, LoadSiteConfigWithExcludedFiles) {
  std::filesystem::path config_path = test_dir() / "site.cfg";
  std::ofstream config_file(config_path);
  config_file << "ExcludedFiles = .git,.gitignore,tags\n";
  config_file.close();

  config::load_site_config(test_dir());

  EXPECT_GE(config::yass_conf.excluded_files.size(), 3);

  // Check that custom excluded files are present
  auto& excluded = config::yass_conf.excluded_files;
  EXPECT_NE(std::find(excluded.begin(), excluded.end(), ".git"),
            excluded.end());
  EXPECT_NE(std::find(excluded.begin(), excluded.end(), ".gitignore"),
            excluded.end());
  EXPECT_NE(std::find(excluded.begin(), excluded.end(), "tags"),
            excluded.end());
}

TEST_F(ConfigTest, LoadSiteConfigWithBooleans) {
  std::filesystem::path config_path = test_dir() / "site.cfg";
  std::ofstream config_file(config_path);
  config_file << "ServerEnabled = true\n";
  config_file << "SitemapEnabled = false\n";
  config_file << "HTMLEnabled = true\n";
  config_file << "StopServerOnError = false\n";
  config_file.close();

  config::load_site_config(test_dir());

  EXPECT_TRUE(config::yass_conf.server_enabled);
  EXPECT_FALSE(config::yass_conf.sitemap_enabled);
  EXPECT_TRUE(config::yass_conf.html_enabled);
  EXPECT_FALSE(config::yass_conf.stop_server_on_error);
}

TEST_F(ConfigTest, LoadSiteConfigWithNumericValues) {
  std::filesystem::path config_path = test_dir() / "site.cfg";
  std::ofstream config_file(config_path);
  config_file << "ServerPort = 9000\n";
  config_file << "AtomFeedAmount = 50\n";
  config_file << "MonitorInterval = 10.5\n";
  config_file << "MonitorConfigInterval = 120.0\n";
  config_file.close();

  config::load_site_config(test_dir());

  EXPECT_EQ(config::yass_conf.server_port, 9000);
  EXPECT_EQ(config::yass_conf.atom_feed_amount, 50);
  EXPECT_DOUBLE_EQ(config::yass_conf.monitor_interval.count(), 10.5);
  EXPECT_DOUBLE_EQ(config::yass_conf.monitor_config_interval.count(), 120.0);
}

TEST_F(ConfigTest, LoadSiteConfigWithSiteTags) {
  std::filesystem::path config_path = test_dir() / "site.cfg";
  std::ofstream config_file(config_path);
  config_file << "Name = Test Site\n";
  config_file << "Language = en\n";
  config_file << "BaseURL = http://test.com\n";
  config_file << "CustomTag = CustomValue\n";
  config_file.close();

  config::load_site_config(test_dir());

  EXPECT_EQ(config::site_tags["Name"], "Test Site");
  EXPECT_EQ(config::site_tags["Language"], "en");
  EXPECT_EQ(config::site_tags["BaseURL"], "http://test.com");
  EXPECT_EQ(config::site_tags["CustomTag"], "CustomValue");
}

TEST_F(ConfigTest, LoadSiteConfigWithCompositeTags) {
  std::filesystem::path config_path = test_dir() / "site.cfg";
  std::ofstream config_file(config_path);
  config_file << "MyList = []\n";
  config_file << "MyList = Item1\n";
  config_file << "MyList = Item2\n";
  config_file << "MyList = Item3\n";
  config_file.close();

  config::load_site_config(test_dir());

  ASSERT_TRUE(config::global_table_tags.find("MyList") !=
              config::global_table_tags.end());
  const auto& my_list = config::global_table_tags["MyList"];
  EXPECT_EQ(my_list.size(), 3);
  EXPECT_EQ(my_list[0], "Item1");
  EXPECT_EQ(my_list[1], "Item2");
  EXPECT_EQ(my_list[2], "Item3");
}

TEST_F(ConfigTest, LoadSiteConfigWithComments) {
  std::filesystem::path config_path = test_dir() / "site.cfg";
  std::ofstream config_file(config_path);
  config_file << "# This is a comment\n";
  config_file << "Name = Test Site\n";
  config_file << "# Another comment\n";
  config_file << "Author = Test Author\n";
  config_file.close();

  config::load_site_config(test_dir());

  EXPECT_EQ(config::yass_conf.site_name, "Test Site");
  EXPECT_EQ(config::yass_conf.author_name, "Test Author");
}

TEST_F(ConfigTest, LoadSiteConfigWithEmptyLines) {
  std::filesystem::path config_path = test_dir() / "site.cfg";
  std::ofstream config_file(config_path);
  config_file << "Name = Test Site\n";
  config_file << "\n";
  config_file << "\n";
  config_file << "Author = Test Author\n";
  config_file.close();

  config::load_site_config(test_dir());

  EXPECT_EQ(config::yass_conf.site_name, "Test Site");
  EXPECT_EQ(config::yass_conf.author_name, "Test Author");
}

TEST_F(ConfigTest, LoadSiteConfigInvalidPort) {
  std::filesystem::path config_path = test_dir() / "site.cfg";
  std::ofstream config_file(config_path);
  config_file << "ServerPort = 70000\n";
  config_file.close();

  EXPECT_THROW(config::load_site_config(test_dir()),
               config::InvalidConfigData);
}

TEST_F(ConfigTest, LoadSiteConfigInvalidFormat) {
  std::filesystem::path config_path = test_dir() / "site.cfg";
  std::ofstream config_file(config_path);
  config_file << "InvalidLine without equals sign\n";
  config_file.close();

  EXPECT_THROW(config::load_site_config(test_dir()),
               config::InvalidConfigData);
}

TEST_F(ConfigTest, LoadSiteConfigNonExistent) {
  std::filesystem::path non_existent = test_dir() / "nonexistent";
  EXPECT_THROW(config::load_site_config(non_existent),
               config::InvalidConfigData);
}

TEST_F(ConfigTest, NormalizeDirectoryPaths) {
  std::filesystem::path config_path = test_dir() / "site.cfg";
  std::ofstream config_file(config_path);
  config_file << "LayoutsDirectory = _layouts\n";
  config_file << "OutputDirectory = _output\n";
  config_file << "ModulesDirectory = _modules\n";
  config_file.close();

  config::load_site_config(test_dir());

  // Paths should be normalized to absolute paths
  EXPECT_TRUE(config::yass_conf.layouts_directory.is_absolute());
  EXPECT_TRUE(config::yass_conf.output_directory.is_absolute());
  EXPECT_TRUE(config::yass_conf.modules_directory.is_absolute());

  // Check that paths contain the test directory
  EXPECT_NE(config::yass_conf.layouts_directory.string().find(test_dir().string()),
            std::string::npos);
}

TEST_F(ConfigTest, TagSeparators) {
  std::filesystem::path config_path = test_dir() / "site.cfg";
  std::ofstream config_file(config_path);
  config_file << "StartTagSeparator = {{\n";
  config_file << "EndTagSeparator = }}\n";
  config_file.close();

  config::load_site_config(test_dir());

  EXPECT_EQ(config::yass_conf.start_tag_separator, "{{");
  EXPECT_EQ(config::yass_conf.end_tag_separator, "}}");
}

TEST_F(ConfigTest, AtomFeedSourceConversion) {
  std::filesystem::path config_path = test_dir() / "site.cfg";
  std::ofstream config_file(config_path);
  config_file << "AtomFeedSource = feed.md\n";
  config_file.close();

  config::load_site_config(test_dir());

  // .md should be converted to .html
  EXPECT_EQ(config::yass_conf.atom_feed_source, "feed.html");
}

TEST_F(ConfigTest, BrowserCommandReplacement) {
  std::filesystem::path config_path = test_dir() / "site.cfg";
  std::ofstream config_file(config_path);
  config_file << "ServerPort = 8080\n";
  config_file << "BrowserCommand = firefox %s\n";
  config_file.close();

  config::load_site_config(test_dir());

  // %s should be replaced with server URL
  EXPECT_EQ(config::yass_conf.browser_command, "firefox http://localhost:8080");
}

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
