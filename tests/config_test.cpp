// Copyright 2019-2021 Bartek thindil Jasicki & 2022-2024 A.J. Ianozi
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

#include <filesystem>
#include <fstream>

#include <gtest/gtest.h>

namespace yass {
namespace {

class ConfigTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create a temporary test directory
        test_dir_ = std::filesystem::temp_directory_path() / "yass_config_test";
        std::filesystem::create_directories(test_dir_);

        // Reset global configuration to defaults
        yass_conf = ParserConfig();
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

    std::filesystem::path test_dir_;
};

TEST_F(ConfigTest, CreateSiteConfigCreatesFile) {
    create_site_config(test_dir_.string());

    auto config_path = test_dir_ / "site.cfg";
    EXPECT_TRUE(std::filesystem::exists(config_path));

    // Verify the file is not empty
    std::ifstream file(config_path);
    ASSERT_TRUE(file.is_open());

    std::string line;
    bool has_content = false;
    while (std::getline(file, line)) {
        if (!line.empty() && line[0] != '#') {
            has_content = true;
            break;
        }
    }
    EXPECT_TRUE(has_content);
}

TEST_F(ConfigTest, LoadSiteConfigBasicSettings) {
    // Create a simple config file
    auto config_path = test_dir_ / "site.cfg";
    std::ofstream file(config_path);
    file << "Name = Test Site\n";
    file << "Language = de\n";
    file << "Author = Jane Doe\n";
    file << "ServerPort = 9000\n";
    file << "ServerEnabled = false\n";
    file << "MonitorInterval = 10.0\n";
    file.close();

    load_site_config(test_dir_.string());

    EXPECT_EQ(yass_conf.site_name, "Test Site");
    EXPECT_EQ(yass_conf.language, "de");
    EXPECT_EQ(yass_conf.author_name, "Jane Doe");
    EXPECT_EQ(yass_conf.server_port, 9000U);
    EXPECT_FALSE(yass_conf.server_enabled);
    EXPECT_EQ(yass_conf.monitor_interval.count(), 10.0);
}

TEST_F(ConfigTest, LoadSiteConfigExcludedFiles) {
    auto config_path = test_dir_ / "site.cfg";
    std::ofstream file(config_path);
    file << "ExcludedFiles = .git,.svn,temp\n";
    file.close();

    load_site_config(test_dir_.string());

    // Should have the three we specified, plus default exclusions
    EXPECT_GE(yass_conf.excluded_files.size(), 3U);

    // Check our specified files are present
    bool has_git = false;
    bool has_svn = false;
    bool has_temp = false;
    for (const auto& excluded : yass_conf.excluded_files) {
        if (excluded == ".git") {
            has_git = true;
        }
        if (excluded == ".svn") {
            has_svn = true;
        }
        if (excluded == "temp") {
            has_temp = true;
        }
    }
    EXPECT_TRUE(has_git);
    EXPECT_TRUE(has_svn);
    EXPECT_TRUE(has_temp);
}

TEST_F(ConfigTest, LoadSiteConfigSiteTags) {
    auto config_path = test_dir_ / "site.cfg";
    std::ofstream file(config_path);
    file << "Name = My Site\n";
    file << "BaseURL = https://example.com\n";
    file << "CustomTag = CustomValue\n";
    file.close();

    load_site_config(test_dir_.string());

    EXPECT_EQ(site_tags["Name"], "My Site");
    EXPECT_EQ(site_tags["BaseURL"], "https://example.com");
    EXPECT_EQ(site_tags["CustomTag"], "CustomValue");
}

TEST_F(ConfigTest, LoadSiteConfigTableTags) {
    auto config_path = test_dir_ / "site.cfg";
    std::ofstream file(config_path);
    file << "MyList = []\n";
    file << "MyList = Item1\n";
    file << "MyList = Item2\n";
    file << "MyList = Item3\n";
    file.close();

    load_site_config(test_dir_.string());

    ASSERT_TRUE(global_table_tags.contains("MyList"));
    EXPECT_EQ(global_table_tags["MyList"].size(), 3U);
    EXPECT_EQ(global_table_tags["MyList"][0], "Item1");
    EXPECT_EQ(global_table_tags["MyList"][1], "Item2");
    EXPECT_EQ(global_table_tags["MyList"][2], "Item3");
}

TEST_F(ConfigTest, LoadSiteConfigInvalidPort) {
    auto config_path = test_dir_ / "site.cfg";
    std::ofstream file(config_path);
    file << "ServerPort = 70000\n";
    file.close();

    EXPECT_THROW(load_site_config(test_dir_.string()), InvalidConfigData);
}

TEST_F(ConfigTest, LoadSiteConfigMissingEquals) {
    auto config_path = test_dir_ / "site.cfg";
    std::ofstream file(config_path);
    file << "InvalidLine without equals\n";
    file.close();

    EXPECT_THROW(load_site_config(test_dir_.string()), InvalidConfigData);
}

TEST_F(ConfigTest, LoadSiteConfigBrowserCommand) {
    auto config_path = test_dir_ / "site.cfg";
    std::ofstream file(config_path);
    file << "ServerPort = 8080\n";
    file << "BrowserCommand = firefox %s/index.html\n";
    file.close();

    load_site_config(test_dir_.string());

    EXPECT_EQ(yass_conf.browser_command, "firefox http://localhost:8080/index.html");
}

TEST_F(ConfigTest, LoadSiteConfigSkipsComments) {
    auto config_path = test_dir_ / "site.cfg";
    std::ofstream file(config_path);
    file << "# This is a comment\n";
    file << "Name = Real Value\n";
    file << "# Another comment\n";
    file.close();

    load_site_config(test_dir_.string());

    EXPECT_EQ(yass_conf.site_name, "Real Value");
}

TEST_F(ConfigTest, SiteDirectoryIsSet) {
    auto config_path = test_dir_ / "site.cfg";
    std::ofstream file(config_path);
    file << "Name = Test\n";
    file.close();

    load_site_config(test_dir_.string());

    EXPECT_EQ(site_directory, test_dir_.string());
}

}  // namespace
}  // namespace yass
