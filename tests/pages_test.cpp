#include <gtest/gtest.h>

#include <filesystem>
#include <fstream>
#include <string>

#include "pages.hpp"

// Test fixture for pages tests
class PagesTest : public ::testing::Test {
 protected:
  void SetUp() override {
    // Create a temporary test directory
    test_dir_ = std::filesystem::temp_directory_path() / "yass_pages_test";
    std::filesystem::create_directories(test_dir_);
  }

  void TearDown() override {
    // Clean up test directory
    if (std::filesystem::exists(test_dir_)) {
      std::filesystem::remove_all(test_dir_);
    }
  }

  std::filesystem::path test_dir_;
};

// Test creating an empty markdown file with .md extension
TEST_F(PagesTest, CreateEmptyFileWithMdExtension) {
  std::filesystem::path test_file = test_dir_ / "test.md";

  // Create the empty file
  pages::create_empty_file(test_file);

  // Verify the file exists
  ASSERT_TRUE(std::filesystem::exists(test_file))
      << "File should exist at: " << test_file;

  // Read the file content
  std::ifstream file(test_file);
  ASSERT_TRUE(file.is_open()) << "Should be able to open the file";

  std::string content((std::istreambuf_iterator<char>(file)),
                      std::istreambuf_iterator<char>());
  file.close();

  // Verify the file contains expected content
  EXPECT_FALSE(content.empty()) << "File should not be empty";

  // Check for key elements in the generated file
  EXPECT_NE(content.find("layout: default"), std::string::npos)
      << "File should contain default layout tag";
  EXPECT_NE(content.find("title: New page"), std::string::npos)
      << "File should contain title tag";
  EXPECT_NE(content.find("All lines which starts with double minus sign"),
            std::string::npos)
      << "File should contain introductory comment";
  EXPECT_NE(content.find("You can without problem delete all this comments"),
            std::string::npos)
      << "File should contain ending comment";

  // Verify comment markers are present (default is "--")
  EXPECT_NE(content.find("-- "), std::string::npos)
      << "File should contain comment markers";
}

// Test creating an empty file in a directory (creates index.md)
TEST_F(PagesTest, CreateEmptyFileInDirectory) {
  std::filesystem::path test_dir = test_dir_ / "testpage";

  // Create the empty file (should create testpage/index.md)
  pages::create_empty_file(test_dir);

  // Verify the directory and index.md exist
  std::filesystem::path index_file = test_dir / "index.md";
  ASSERT_TRUE(std::filesystem::exists(index_file))
      << "index.md should exist at: " << index_file;

  // Read and verify content
  std::ifstream file(index_file);
  ASSERT_TRUE(file.is_open());

  std::string content((std::istreambuf_iterator<char>(file)),
                      std::istreambuf_iterator<char>());
  file.close();

  EXPECT_FALSE(content.empty());
  EXPECT_NE(content.find("layout: default"), std::string::npos);
}

// Test that create_empty_file creates parent directories if needed
TEST_F(PagesTest, CreateEmptyFileCreatesParentDirectories) {
  std::filesystem::path nested_file =
      test_dir_ / "level1" / "level2" / "test.md";

  // Create the file (should create parent directories)
  pages::create_empty_file(nested_file);

  // Verify the file exists
  ASSERT_TRUE(std::filesystem::exists(nested_file))
      << "File should exist at: " << nested_file;

  // Verify parent directories were created
  EXPECT_TRUE(std::filesystem::exists(nested_file.parent_path()));
}

// Test that the generated file contains documentation about sitemap
TEST_F(PagesTest, CreateEmptyFileContainsSitemapDocumentation) {
  std::filesystem::path test_file = test_dir_ / "test.md";

  pages::create_empty_file(test_file);

  std::ifstream file(test_file);
  std::string content((std::istreambuf_iterator<char>(file)),
                      std::istreambuf_iterator<char>());
  file.close();

  // Check for sitemap documentation
  EXPECT_NE(content.find("priority"), std::string::npos)
      << "File should contain sitemap priority documentation";
  EXPECT_NE(content.find("changefreq"), std::string::npos)
      << "File should contain sitemap changefreq documentation";
  EXPECT_NE(content.find("insitemap"), std::string::npos)
      << "File should mention insitemap option";
}

// Test that the generated file contains documentation about Atom feed
TEST_F(PagesTest, CreateEmptyFileContainsAtomFeedDocumentation) {
  std::filesystem::path test_file = test_dir_ / "test.md";

  pages::create_empty_file(test_file);

  std::ifstream file(test_file);
  std::string content((std::istreambuf_iterator<char>(file)),
                      std::istreambuf_iterator<char>());
  file.close();

  // Check for Atom feed documentation
  EXPECT_NE(content.find("Atom feed"), std::string::npos)
      << "File should contain Atom feed documentation";
  EXPECT_NE(content.find("author"), std::string::npos)
      << "File should mention author tag";
  EXPECT_NE(content.find("summary"), std::string::npos)
      << "File should mention summary tag";
}
