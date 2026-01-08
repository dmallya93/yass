from conan import ConanFile
from conan.tools.cmake import cmake_layout, CMakeDeps, CMakeToolchain

class YassConan(ConanFile):
    name = "yass"
    version = "0.1.0"
    settings = "os", "compiler", "build_type", "arch"

    def requirements(self):
        self.requires("gtest/1.14.0")

    def build_requirements(self):
        self.tool_requires("cmake/3.27.1")

    def configure(self):
        # Configure options based on Ada migration needs
        self.options["gtest"].shared = False

        # Optimize for safety-critical code
        if self.settings.build_type == "Debug":
            pass  # Add debug-specific options if needed

    def layout(self):
        cmake_layout(self)

    def generate(self):
        deps = CMakeDeps(self)
        deps.generate()
        tc = CMakeToolchain(self)
        tc.generate()
