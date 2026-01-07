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

#include <gtest/gtest.h>

#include <array>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <memory>
#include <string>

namespace fs = std::filesystem;

// Get the path to the yass executable
std::string get_yass_path() {
  // The test is run from the build directory, so yass should be in the same directory
  fs::path test_exe_path = fs::read_symlink("/proc/self/exe");
  fs::path build_dir = test_exe_path.parent_path();
  fs::path yass_path = build_dir / "yass";
  return yass_path.string();
}

// Helper function to execute a command and capture output
std::string exec_command(const std::string& cmd) {
  std::array<char, 128> buffer{};
  std::string result;
  // NOLINTNEXTLINE(cert-env33-c)
  std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd.c_str(), "r"),
                                                  pclose);
  if (!pipe) {
    throw std::runtime_error("popen() failed!");
  }
  while (fgets(buffer.data(), static_cast<int>(buffer.size()), pipe.get()) !=
         nullptr) {
    result += buffer.data();
  }
  return result;
}

// Test help command
TEST(CliTest, HelpCommand) {
  std::string cmd = get_yass_path() + " help 2>&1";
  std::string output = exec_command(cmd);

  EXPECT_NE(output.find("Possible actions:"), std::string::npos);
  EXPECT_NE(output.find("help - show this screen and exit"), std::string::npos);
  EXPECT_NE(output.find("version - show the program version and exit"),
            std::string::npos);
  EXPECT_NE(output.find("license - show short info about the program license"),
            std::string::npos);
  EXPECT_NE(output.find("readme - show content of README file"),
            std::string::npos);
  EXPECT_NE(output.find("createnow [name]"), std::string::npos);
  EXPECT_NE(output.find("create [name]"), std::string::npos);
  EXPECT_NE(output.find("build [name]"), std::string::npos);
  EXPECT_NE(output.find("server [name]"), std::string::npos);
  EXPECT_NE(output.find("createfile [name]"), std::string::npos);
}

// Test help command with no arguments
TEST(CliTest, NoArgumentsShowsHelp) {
  std::string cmd = get_yass_path() + " 2>&1";
  std::string output = exec_command(cmd);

  EXPECT_NE(output.find("Possible actions:"), std::string::npos);
}

// Test version command
TEST(CliTest, VersionCommand) {
  std::string cmd = get_yass_path() + " version 2>&1";
  std::string output = exec_command(cmd);

  EXPECT_NE(output.find("Version:"), std::string::npos);
  EXPECT_NE(output.find("Released:"), std::string::npos);
  EXPECT_NE(output.find("3.2.0-dev"), std::string::npos);
}

// Test license command
TEST(CliTest, LicenseCommand) {
  std::string cmd = get_yass_path() + " license 2>&1";
  std::string output = exec_command(cmd);

  EXPECT_NE(output.find("Copyright"), std::string::npos);
  EXPECT_NE(output.find("GNU General Public License"), std::string::npos);
  EXPECT_NE(output.find("A.J. Ianozi"), std::string::npos);
  EXPECT_NE(output.find("Bartek thindil Jasicki"), std::string::npos);
}

// Test readme command
TEST(CliTest, ReadmeCommand) {
  std::string cmd = get_yass_path() + " readme 2>&1";
  std::string output = exec_command(cmd);

  EXPECT_NE(output.find("YASS"), std::string::npos);
  EXPECT_NE(output.find("static site"), std::string::npos);
}

// Test createnow command
TEST(CliTest, CreateNowCommand) {
  // Clean up any previous test directory
  fs::path test_dir = fs::temp_directory_path() / "test_site_createnow";
  fs::remove_all(test_dir);

  // Run createnow command
  std::string cmd =
      get_yass_path() + " createnow " + test_dir.string() + " 2>&1";
  std::string output = exec_command(cmd);

  // Check that success message appears
  EXPECT_NE(output.find("was created"), std::string::npos);

  // Verify directory structure was created
  EXPECT_TRUE(fs::exists(test_dir));
  EXPECT_TRUE(fs::exists(test_dir / "_layouts"));
  EXPECT_TRUE(fs::exists(test_dir / "_output"));
  EXPECT_TRUE(fs::exists(test_dir / "_modules"));
  EXPECT_TRUE(fs::exists(test_dir / "_modules" / "start"));
  EXPECT_TRUE(fs::exists(test_dir / "_modules" / "pre"));
  EXPECT_TRUE(fs::exists(test_dir / "_modules" / "post"));
  EXPECT_TRUE(fs::exists(test_dir / "_modules" / "end"));
  EXPECT_TRUE(fs::exists(test_dir / "site.cfg"));

  // Verify site.cfg content
  std::ifstream config_file(test_dir / "site.cfg");
  ASSERT_TRUE(config_file.is_open());
  std::string config_content;
  std::string line;
  while (std::getline(config_file, line)) {
    config_content += line + "\n";
  }

  EXPECT_NE(config_content.find("LayoutsDirectory = _layouts"),
            std::string::npos);
  EXPECT_NE(config_content.find("OutputDirectory = _output"),
            std::string::npos);
  EXPECT_NE(config_content.find("Name = New Site"), std::string::npos);
  EXPECT_NE(config_content.find("Author = John Doe"), std::string::npos);

  // Clean up
  fs::remove_all(test_dir);
}

// Test that createnow fails if directory already exists
TEST(CliTest, CreateNowFailsIfDirectoryExists) {
  fs::path test_dir = fs::temp_directory_path() / "test_site_exists";
  fs::create_directories(test_dir);

  std::string cmd = get_yass_path() + " createnow " + test_dir.string() + " 2>&1";
  std::string output = exec_command(cmd);

  EXPECT_NE(output.find("Directory with that name exists"), std::string::npos);

  fs::remove_all(test_dir);
}

// Test build command shows not implemented message
TEST(CliTest, BuildCommandNotImplemented) {
  fs::path test_dir = fs::temp_directory_path() / "test_site_build";
  fs::create_directories(test_dir);
  // Create a minimal site.cfg
  std::ofstream config(test_dir / "site.cfg");
  config << "Name = Test\n";
  config.close();

  std::string cmd = get_yass_path() + " build " + test_dir.string() + " 2>&1";
  std::string output = exec_command(cmd);

  EXPECT_NE(output.find("not yet implemented"), std::string::npos);

  fs::remove_all(test_dir);
}

// Test server command shows not implemented message
TEST(CliTest, ServerCommandNotImplemented) {
  fs::path test_dir = fs::temp_directory_path() / "test_site_server";
  fs::create_directories(test_dir);
  // Create a minimal site.cfg
  std::ofstream config(test_dir / "site.cfg");
  config << "Name = Test\n";
  config.close();

  std::string cmd = get_yass_path() + " server " + test_dir.string() + " 2>&1";
  std::string output = exec_command(cmd);

  EXPECT_NE(output.find("not yet implemented"), std::string::npos);

  fs::remove_all(test_dir);
}

// Test createfile command shows not implemented message
TEST(CliTest, CreateFileCommandNotImplemented) {
  std::string cmd = get_yass_path() + " createfile test.md 2>&1";
  std::string output = exec_command(cmd);

  EXPECT_NE(output.find("not yet implemented"), std::string::npos);
}

// Test invalid command shows error
TEST(CliTest, InvalidCommandShowsError) {
  std::string cmd = get_yass_path() + " invalidcmd 2>&1";
  std::string output = exec_command(cmd);

  // CLI11 will show an error message for invalid subcommands
  EXPECT_NE(output.find("not expected"), std::string::npos);
}
