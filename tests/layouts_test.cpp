#include "layouts.hpp"

#include <gtest/gtest.h>

#include <filesystem>
#include <fstream>
#include <string>

namespace fs = std::filesystem;

class LayoutsTest : public ::testing::Test {
 protected:
  void SetUp() override {
    // Create a temporary test directory
    test_dir = fs::temp_directory_path() / "yass_test";
    fs::create_directories(test_dir);
  }

  void TearDown() override {
    // Clean up test directory
    if (fs::exists(test_dir)) {
      fs::remove_all(test_dir);
    }
  }

  std::string read_file(const fs::path& path) {
    std::ifstream file(path);
    if (!file) {
      throw std::runtime_error("Failed to read file: " + path.string());
    }
    return std::string(std::istreambuf_iterator<char>(file),
                       std::istreambuf_iterator<char>());
  }

  fs::path test_dir;
};

TEST_F(LayoutsTest, CreateLayoutGeneratesDefaultHtml) {
  layouts::create_layout(test_dir);

  const auto layout_path = test_dir / "_layouts" / "default.html";
  ASSERT_TRUE(fs::exists(layout_path))
      << "Failed to create default.html layout file";

  const std::string content = read_file(layout_path);

  // Verify HTML5 structure
  EXPECT_NE(content.find("<!DOCTYPE html>"), std::string::npos);
  EXPECT_NE(content.find("<html"), std::string::npos);
  EXPECT_NE(content.find("</html>"), std::string::npos);

  // Verify meta tags
  EXPECT_NE(content.find("<meta charset=\"UTF-8\">"), std::string::npos);
  EXPECT_NE(content.find("<meta name=\"viewport\""), std::string::npos);

  // Verify M1 template tags (simple substitution only)
  EXPECT_NE(content.find("{%Language%}"), std::string::npos);
  EXPECT_NE(content.find("{%Name%}"), std::string::npos);
  EXPECT_NE(content.find("{%Content%}"), std::string::npos);

  // M1 does not include AWS template syntax (@@IF@@, etc.)
  // These are deferred to Milestone 2
  EXPECT_EQ(content.find("@@IF@@"), std::string::npos)
      << "M1 layouts should not contain @@IF@@ syntax";
  EXPECT_EQ(content.find("@@END_IF@@"), std::string::npos)
      << "M1 layouts should not contain @@END_IF@@ syntax";

  // M1 does not include optional tags that may not have values
  EXPECT_EQ(content.find("{%canonicallink%}"), std::string::npos)
      << "M1 layouts should not include optional tags without defaults";
  EXPECT_EQ(content.find("{%author%}"), std::string::npos)
      << "M1 layouts should not include optional tags without defaults";
  EXPECT_EQ(content.find("{%description%}"), std::string::npos)
      << "M1 layouts should not include optional tags without defaults";
  EXPECT_EQ(content.find("{%AtomLink%}"), std::string::npos)
      << "M1 layouts should not include optional tags without defaults";
}

TEST_F(LayoutsTest, CreateDirectoryLayoutGeneratesDirectoryHtml) {
  layouts::create_directory_layout(test_dir);

  const auto layout_path = test_dir / "_layouts" / "directory.html";
  ASSERT_TRUE(fs::exists(layout_path))
      << "Failed to create directory.html layout file";

  const std::string content = read_file(layout_path);

  // Verify HTML structure
  EXPECT_NE(content.find("<!DOCTYPE html>"), std::string::npos);
  EXPECT_NE(content.find("<html>"), std::string::npos);
  EXPECT_NE(content.find("</html>"), std::string::npos);

  // Verify meta tags
  EXPECT_NE(content.find("<meta charset=\"UTF-8\">"), std::string::npos);
  EXPECT_NE(content.find("<meta name=\"viewport\""), std::string::npos);

  // Verify body structure
  EXPECT_NE(content.find("<body>"), std::string::npos);
  EXPECT_NE(content.find("</body>"), std::string::npos);

  // M1 does not implement @@TABLE@@ loops - deferred to Milestone 2
  EXPECT_EQ(content.find("@@TABLE@@"), std::string::npos)
      << "M1 layouts should not contain @@TABLE@@ syntax";
  EXPECT_EQ(content.find("@@END_TABLE@@"), std::string::npos)
      << "M1 layouts should not contain @@END_TABLE@@ syntax";
  EXPECT_EQ(content.find("{%NAME_V%}"), std::string::npos)
      << "M1 layouts should not include table iteration variables";

  // Verify it's a placeholder for M2
  EXPECT_NE(content.find("Directory"), std::string::npos)
      << "Directory layout should mention directory listing";
  EXPECT_NE(content.find("Milestone 2"), std::string::npos)
      << "Directory layout should indicate M2 implementation";
}

TEST_F(LayoutsTest, CreateLayoutCreatesLayoutsDirectory) {
  // Ensure _layouts directory doesn't exist initially
  const auto layouts_dir = test_dir / "_layouts";
  ASSERT_FALSE(fs::exists(layouts_dir));

  layouts::create_layout(test_dir);

  EXPECT_TRUE(fs::exists(layouts_dir)) << "_layouts directory was not created";
  EXPECT_TRUE(fs::is_directory(layouts_dir));
}

TEST_F(LayoutsTest, CreateLayoutThrowsOnInvalidPath) {
  // Test with a path that cannot be created (e.g., nested under a file)
  const auto test_file = test_dir / "test.txt";
  std::ofstream(test_file) << "test";

  const auto invalid_dir = test_file / "subdir";
  EXPECT_THROW(layouts::create_layout(invalid_dir), std::exception);
}
