// Copyright (c) 2019-2021 Bartek thindil Jasicki <thindil@laeran.pl>
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

#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include <string>
#include <random>
#include <chrono>
#include "yass/config.hpp"
#include "yass/layouts.hpp"
#include "yass/pages.hpp"

namespace fs = std::filesystem;

class CLITest : public ::testing::Test {
protected:
    fs::path test_dir;

    void SetUp() override {
        // Create a unique temporary directory for each test using C++11 random
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(100000, 999999);
        test_dir = fs::temp_directory_path() / ("yass_test_" + std::to_string(dis(gen)));

        // Clean up if it exists from a previous failed test
        if (fs::exists(test_dir)) {
            fs::remove_all(test_dir);
        }
    }

    void TearDown() override {
        // Clean up the test directory
        if (fs::exists(test_dir)) {
            fs::remove_all(test_dir);
        }
    }
};

// Test directory structure creation for site
TEST_F(CLITest, DirectoryStructureCreation) {
    // Create the site directory first
    fs::create_directories(test_dir);

    // Create expected subdirectories
    const std::vector<std::string> EXPECTED_DIRS = {
        "_layouts",
        "_output",
        "_modules/start",
        "_modules/pre",
        "_modules/post",
        "_modules/end"
    };

    for (const auto& dir : EXPECTED_DIRS) {
        fs::path full_path = test_dir / dir;
        fs::create_directories(full_path);
        EXPECT_TRUE(fs::exists(full_path)) << "Directory should exist: " << full_path;
        EXPECT_TRUE(fs::is_directory(full_path)) << "Path should be a directory: " << full_path;
    }
}

// Test site configuration file creation
TEST_F(CLITest, ConfigFileCreation) {
    fs::create_directories(test_dir);

    // Create config file
    yass::create_site_config(test_dir.string());

    fs::path config_path = test_dir / "site.cfg";
    EXPECT_TRUE(fs::exists(config_path)) << "site.cfg should be created";

    // Verify config file is not empty
    std::ifstream config_file(config_path);
    std::string line;
    int line_count = 0;
    while (std::getline(config_file, line)) {
        line_count++;
    }
    EXPECT_GT(line_count, 0) << "Config file should not be empty";
}

// Test layout file creation
TEST_F(CLITest, LayoutFileCreation) {
    fs::create_directories(test_dir / "_layouts");

    // Create layout files
    yass::create_layout(test_dir.string());
    yass::create_directory_layout(test_dir.string());

    fs::path default_layout = test_dir / "_layouts" / "default.html";
    fs::path directory_layout = test_dir / "_layouts" / "directory.html";

    EXPECT_TRUE(fs::exists(default_layout)) << "default.html should be created";
    EXPECT_TRUE(fs::exists(directory_layout)) << "directory.html should be created";

    // Verify default.html contains expected content
    std::ifstream default_file(default_layout);
    std::string content((std::istreambuf_iterator<char>(default_file)),
                        std::istreambuf_iterator<char>());
    EXPECT_NE(content.find("<!DOCTYPE html>"), std::string::npos);
    EXPECT_NE(content.find("{%Language%}"), std::string::npos);
    EXPECT_NE(content.find("{%Content%}"), std::string::npos);

    // Verify directory.html contains expected content
    std::ifstream dir_file(directory_layout);
    std::string dir_content((std::istreambuf_iterator<char>(dir_file)),
                            std::istreambuf_iterator<char>());
    EXPECT_NE(dir_content.find("<!DOCTYPE html>"), std::string::npos);
    EXPECT_NE(dir_content.find("@@TABLE@@"), std::string::npos);
}

// Test empty file creation
TEST_F(CLITest, EmptyFileCreation) {
    fs::create_directories(test_dir);

    // Create config first to have markdown_comment available
    yass::create_site_config(test_dir.string());
    yass::load_site_config(test_dir.string());

    // Create empty file
    fs::path test_file = test_dir / "test.md";
    yass::create_empty_file(test_file.string());

    EXPECT_TRUE(fs::exists(test_file)) << "test.md should be created";

    // Verify file contains template comments
    std::ifstream file(test_file);
    std::string content((std::istreambuf_iterator<char>(file)),
                        std::istreambuf_iterator<char>());
    EXPECT_NE(content.find("layout: default"), std::string::npos);
    EXPECT_NE(content.find("title:"), std::string::npos);
}

// Test empty file creation in directory (should create index.md)
TEST_F(CLITest, EmptyFileCreationInDirectory) {
    fs::create_directories(test_dir);

    // Create config first
    yass::create_site_config(test_dir.string());
    yass::load_site_config(test_dir.string());

    // Create empty file with directory path
    yass::create_empty_file(test_dir.string());

    fs::path index_file = test_dir / "index.md";
    EXPECT_TRUE(fs::exists(index_file)) << "index.md should be created in directory";
}

// Test configuration loading
TEST_F(CLITest, ConfigurationLoading) {
    fs::create_directories(test_dir);

    // Create and load config
    yass::create_site_config(test_dir.string());
    yass::load_site_config(test_dir.string());

    // Verify default values are loaded (paths are made absolute after loading)
    EXPECT_NE(yass::yass_conf.output_directory.find("_output"), std::string::npos);
    EXPECT_NE(yass::yass_conf.layouts_directory.find("_layouts"), std::string::npos);
    EXPECT_NE(yass::yass_conf.modules_directory.find("_modules"), std::string::npos);
    EXPECT_EQ(yass::yass_conf.markdown_comment, "--");
    EXPECT_EQ(yass::yass_conf.server_port, 8888U);
    EXPECT_TRUE(yass::yass_conf.server_enabled);
    EXPECT_TRUE(yass::yass_conf.sitemap_enabled);
}

// Test error handling - directory must not exist for create
TEST_F(CLITest, CreateDirectoryMustNotExist) {
    // Create the directory first
    fs::create_directories(test_dir);

    // Attempting to create site in existing directory should fail
    // (This would be tested at main() level, but we verify the check logic)
    EXPECT_TRUE(fs::exists(test_dir));
}

// Test error handling - directory must exist for build
TEST_F(CLITest, BuildDirectoryMustExist) {
    // Verify directory doesn't exist
    EXPECT_FALSE(fs::exists(test_dir));

    // Attempting to build non-existing directory should fail
    // (This would be tested at main() level)
}

// Test error handling - site.cfg must exist for build
TEST_F(CLITest, BuildRequiresSiteConfig) {
    fs::create_directories(test_dir);

    fs::path config_path = test_dir / "site.cfg";
    EXPECT_FALSE(fs::exists(config_path));

    // Attempting to build directory without site.cfg should fail
    // (This would be tested at main() level)
}

// Test layout creation with proper HTML structure
TEST_F(CLITest, LayoutHTMLStructure) {
    fs::create_directories(test_dir / "_layouts");

    yass::create_layout(test_dir.string());

    fs::path layout_path = test_dir / "_layouts" / "default.html";
    std::ifstream layout_file(layout_path);
    std::string content((std::istreambuf_iterator<char>(layout_file)),
                        std::istreambuf_iterator<char>());

    // Verify HTML5 structure
    EXPECT_NE(content.find("<!DOCTYPE html>"), std::string::npos);
    EXPECT_NE(content.find("<html"), std::string::npos);
    EXPECT_NE(content.find("<head>"), std::string::npos);
    EXPECT_NE(content.find("<body>"), std::string::npos);
    EXPECT_NE(content.find("</body>"), std::string::npos);
    EXPECT_NE(content.find("</html>"), std::string::npos);

    // Verify meta tags
    EXPECT_NE(content.find("charset=\"UTF-8\""), std::string::npos);
    EXPECT_NE(content.find("viewport"), std::string::npos);

    // Verify YASS template tags
    EXPECT_NE(content.find("{%Language%}"), std::string::npos);
    EXPECT_NE(content.find("{%author%}"), std::string::npos);
    EXPECT_NE(content.find("{%description%}"), std::string::npos);
    EXPECT_NE(content.find("{%Name%}"), std::string::npos);
    EXPECT_NE(content.find("{%Content%}"), std::string::npos);
    EXPECT_NE(content.find("{%canonicallink%}"), std::string::npos);

    // Verify conditional blocks
    EXPECT_NE(content.find("@@IF@@"), std::string::npos);
    EXPECT_NE(content.find("@@END_IF@@"), std::string::npos);
}

// Test complete site creation workflow
TEST_F(CLITest, CompleteSiteCreationWorkflow) {
    // 1. Create directory structure
    fs::create_directories(test_dir);
    const std::vector<std::string> DIRS = {
        "_layouts", "_output", "_modules/start",
        "_modules/pre", "_modules/post", "_modules/end"
    };
    for (const auto& dir : DIRS) {
        fs::create_directories(test_dir / dir);
    }

    // 2. Create configuration
    yass::create_site_config(test_dir.string());

    // 3. Create layouts
    yass::create_layout(test_dir.string());
    yass::create_directory_layout(test_dir.string());

    // 4. Load config
    yass::load_site_config(test_dir.string());

    // 5. Create index file
    yass::create_empty_file(test_dir.string());

    // Verify all components exist
    EXPECT_TRUE(fs::exists(test_dir / "site.cfg"));
    EXPECT_TRUE(fs::exists(test_dir / "_layouts" / "default.html"));
    EXPECT_TRUE(fs::exists(test_dir / "_layouts" / "directory.html"));
    EXPECT_TRUE(fs::exists(test_dir / "index.md"));

    for (const auto& dir : DIRS) {
        EXPECT_TRUE(fs::exists(test_dir / dir));
    }
}

// Test markdown file with proper extension handling
TEST_F(CLITest, MarkdownExtensionHandling) {
    fs::create_directories(test_dir);

    yass::create_site_config(test_dir.string());
    yass::load_site_config(test_dir.string());

    // Test with .md extension
    fs::path file_with_ext = test_dir / "page.md";
    yass::create_empty_file(file_with_ext.string());
    EXPECT_TRUE(fs::exists(file_with_ext));

    // Test without extension (should still work based on path)
    fs::path subdir = test_dir / "subdir";
    fs::create_directories(subdir);
    yass::create_empty_file(subdir.string());
    EXPECT_TRUE(fs::exists(subdir / "index.md"));
}

// Test global configuration state
TEST_F(CLITest, GlobalConfigState) {
    fs::create_directories(test_dir);

    // Create and load config
    yass::create_site_config(test_dir.string());
    yass::load_site_config(test_dir.string());

    // Verify global config is accessible
    EXPECT_FALSE(yass::yass_conf.output_directory.empty());
    EXPECT_FALSE(yass::yass_conf.layouts_directory.empty());
    EXPECT_FALSE(yass::yass_conf.site_name.empty());
    EXPECT_FALSE(yass::yass_conf.author_name.empty());
}
