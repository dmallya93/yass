from conan import ConanFile
from conan.tools.cmake import cmake_layout

class YassConan(ConanFile):
    name = "yass"
    version = "3.2.0"
    settings = "os", "compiler", "build_type", "arch"

    def requirements(self):
        # Dependencies will be added as we implement more features
        # For now, just the basics for future milestones:
        # self.requires("boost/1.83.0")
        # self.requires("fmt/10.1.1")
        # self.requires("libcmark/0.31.0")
        pass

    def layout(self):
        cmake_layout(self)
