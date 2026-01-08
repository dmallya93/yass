#include <gtest/gtest.h>

#include <filesystem>
#include <fstream>
#include <string>

#include "config.hpp"
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

// Test create_page processes markdown and substitutes tags
TEST_F(PagesTest, CreatePageProcessesMarkdownAndSubstituteTags) {
  // Setup directories
  std::filesystem::path site_dir = test_dir_ / "site";
  std::filesystem::path layouts_dir = site_dir / "_layouts";
  std::filesystem::path output_dir = site_dir / "_output";
  std::filesystem::create_directories(layouts_dir);
  std::filesystem::create_directories(output_dir);

  // Configure yass_config
  config::yass_config.layouts_directory = layouts_dir.string();
  config::yass_config.output_directory = output_dir.string();
  config::yass_config.markdown_comment = "--";
  config::yass_config.start_tag_separator = "{%";
  config::yass_config.end_tag_separator = "%}";
  config::yass_config.tags["SiteName"] = "Test Site";
  config::site_directory = site_dir;

  // Create a layout file
  std::filesystem::path layout_file = layouts_dir / "default.html";
  std::ofstream layout(layout_file);
  layout << "<!DOCTYPE html>\n";
  layout << "<html>\n";
  layout << "<head><title>{%title%}</title></head>\n";
  layout << "<body>\n";
  layout << "<h1>{%SiteName%}</h1>\n";
  layout << "{%Content%}\n";
  layout << "</body>\n";
  layout << "</html>\n";
  layout.close();

  // Create a markdown file with tags
  std::filesystem::path md_file = site_dir / "test.md";
  std::ofstream md(md_file);
  md << "-- layout: default\n";
  md << "-- title: Test Page\n";
  md << "\n";
  md << "# Welcome\n";
  md << "\n";
  md << "This is **bold** text.\n";
  md.close();

  // Process the page
  pages::create_page(md_file, site_dir);

  // Verify output file exists
  std::filesystem::path output_file = output_dir / "test.html";
  ASSERT_TRUE(std::filesystem::exists(output_file))
      << "Output HTML file should exist at: " << output_file;

  // Read output content
  std::ifstream output(output_file);
  std::string content((std::istreambuf_iterator<char>(output)),
                     std::istreambuf_iterator<char>());
  output.close();

  // Verify markdown was converted to HTML
  EXPECT_NE(content.find("<h1>Welcome</h1>"), std::string::npos)
      << "Should contain converted H1 heading";
  EXPECT_NE(content.find("<strong>bold</strong>"), std::string::npos)
      << "Should contain bold text";

  // Verify page tag substitution
  EXPECT_NE(content.find("<title>Test Page</title>"), std::string::npos)
      << "Should substitute page title tag";

  // Verify site tag substitution
  EXPECT_NE(content.find("<h1>Test Site</h1>"), std::string::npos)
      << "Should substitute site name tag";

  // Verify layout structure
  EXPECT_NE(content.find("<!DOCTYPE html>"), std::string::npos)
      << "Should contain layout HTML structure";
}

// Test copy_file preserves directory structure
TEST_F(PagesTest, CopyFilePreservesDirectoryStructure) {
  // Setup directories
  std::filesystem::path site_dir = test_dir_ / "site";
  std::filesystem::path subdir = site_dir / "images";
  std::filesystem::path output_dir = site_dir / "_output";
  std::filesystem::create_directories(subdir);
  std::filesystem::create_directories(output_dir);

  // Configure yass_config
  config::yass_config.output_directory = output_dir.string();

  // Create a test file in subdirectory
  std::filesystem::path source_file = subdir / "test.png";
  std::ofstream source(source_file, std::ios::binary);
  source << "PNG fake data";
  source.close();

  // Copy the file
  pages::copy_file(source_file, site_dir);

  // Verify file copied to correct location
  std::filesystem::path output_file = output_dir / "images" / "test.png";
  ASSERT_TRUE(std::filesystem::exists(output_file))
      << "Copied file should exist at: " << output_file;

  // Verify content matches
  std::ifstream output(output_file, std::ios::binary);
  std::string content((std::istreambuf_iterator<char>(output)),
                     std::istreambuf_iterator<char>());
  output.close();

  EXPECT_EQ(content, "PNG fake data")
      << "Copied file content should match source";
}

// Test create_page filters comment lines from content
TEST_F(PagesTest, CreatePageFiltersCommentLines) {
  // Setup directories
  std::filesystem::path site_dir = test_dir_ / "site";
  std::filesystem::path layouts_dir = site_dir / "_layouts";
  std::filesystem::path output_dir = site_dir / "_output";
  std::filesystem::create_directories(layouts_dir);
  std::filesystem::create_directories(output_dir);

  // Configure yass_config
  config::yass_config.layouts_directory = layouts_dir.string();
  config::yass_config.output_directory = output_dir.string();
  config::yass_config.markdown_comment = "--";
  config::yass_config.start_tag_separator = "{%";
  config::yass_config.end_tag_separator = "%}";

  // Create a simple layout
  std::filesystem::path layout_file = layouts_dir / "default.html";
  std::ofstream layout(layout_file);
  layout << "<html><body>{%Content%}</body></html>\n";
  layout.close();

  // Create markdown with comments
  std::filesystem::path md_file = site_dir / "test.md";
  std::ofstream md(md_file);
  md << "-- layout: default\n";
  md << "-- This is a comment without a tag\n";
  md << "-- Another comment\n";
  md << "\n";
  md << "This is actual content.\n";
  md.close();

  // Process the page
  pages::create_page(md_file, site_dir);

  // Read output
  std::filesystem::path output_file = output_dir / "test.html";
  std::ifstream output(output_file);
  std::string content((std::istreambuf_iterator<char>(output)),
                     std::istreambuf_iterator<char>());
  output.close();

  // Verify comment lines are not in output
  EXPECT_EQ(content.find("This is a comment"), std::string::npos)
      << "Comment lines should be filtered out";
  EXPECT_EQ(content.find("Another comment"), std::string::npos)
      << "Comment lines should be filtered out";

  // Verify actual content is present
  EXPECT_NE(content.find("This is actual content"), std::string::npos)
      << "Actual content should be present";
}
