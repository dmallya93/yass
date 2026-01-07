from conans import ConanFile, CMake


class YASSConan(ConanFile):
    name = "yass"
    version = "1.0.0"
    settings = "os", "compiler", "build_type", "arch"
    generators = "cmake_find_package"

    def requirements(self):
        # spdlog for logging with colored output support
        self.requires("spdlog/1.12.0")
        # CLI11 for command-line argument parsing
        self.requires("cli11/2.3.2")
        # gtest for unit testing
        self.requires("gtest/1.14.0")
