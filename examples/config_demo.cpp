// Simple demonstration of the YASS configuration module
// This example shows how to create, save, and load configuration

#include "yass/config.hpp"
#include "yass/messages.hpp"

#include <filesystem>
#include <iostream>

int main() {
    namespace fs = std::filesystem;

    // Create a temporary demo directory
    fs::path demo_dir = fs::temp_directory_path() / "yass_demo";
    fs::create_directories(demo_dir);

    std::cout << "YASS Configuration Module Demo\n";
    std::cout << "==============================\n\n";

    // Set some custom configuration values
    yass::yass_conf.site_name = "My Demo Site";
    yass::yass_conf.author_name = "Demo Author";
    yass::yass_conf.language = "en";
    yass::yass_conf.server_port = 9000;

    std::cout << "Creating configuration file in: " << demo_dir << "\n";

    try {
        // Create the configuration file
        yass::create_site_config(demo_dir.string());
        yass::show_message("Configuration file created successfully!", yass::MessageType::SUCCESS);

        // Load the configuration back
        std::cout << "\nLoading configuration...\n";
        yass::load_site_config(demo_dir.string());
        yass::show_message("Configuration loaded successfully!", yass::MessageType::SUCCESS);

        // Display loaded configuration
        std::cout << "\nLoaded Configuration:\n";
        std::cout << "  Site Name: " << yass::yass_conf.site_name << "\n";
        std::cout << "  Author: " << yass::yass_conf.author_name << "\n";
        std::cout << "  Language: " << yass::yass_conf.language << "\n";
        std::cout << "  Server Port: " << yass::yass_conf.server_port << "\n";
        std::cout << "  Output Directory: " << yass::yass_conf.output_directory << "\n";
        std::cout << "  Layouts Directory: " << yass::yass_conf.layouts_directory << "\n";

        std::cout << "\nSite Tags:\n";
        for (const auto& [key, value] : yass::site_tags) {
            std::cout << "  " << key << " = " << value << "\n";
        }

    } catch (const std::exception& e) {
        yass::show_message(std::string("Error: ") + e.what(), yass::MessageType::ERROR);
        return 1;
    }

    // Cleanup
    fs::remove_all(demo_dir);

    std::cout << "\nDemo completed successfully!\n";
    return 0;
}
