#include "pages.hpp"

#include <algorithm>
#include <fstream>
#include <sstream>
#include <stdexcept>

#include "cmark_wrapper.hpp"
#include "config.hpp"
#include "messages.hpp"

namespace pages {

// Helper function to write a line with comment prefix
static void write_comment_line(std::ofstream& file,
                                const std::string& comment_marker,
                                const std::string& text) {
  file << comment_marker << " " << text << '\n';
}

void create_empty_file(const std::filesystem::path& file_name) {
  std::filesystem::path target_path;

  // Determine target file path
  if (file_name.extension() == ".md") {
    target_path = file_name;
  } else {
    target_path = file_name / "index.md";
  }

  // Create parent directory if needed
  if (target_path.has_parent_path()) {
    std::filesystem::create_directories(target_path.parent_path());
  }

  // Create and open the file
  std::ofstream index_file(target_path, std::ios::app);
  if (!index_file) {
    throw std::runtime_error("Failed to create file: " + target_path.string());
  }

  const std::string& comment = config::yass_config.markdown_comment;

  // Write all the default content with comments
  write_comment_line(
      index_file, comment,
      "All lines which starts with double minus sign are comments and ignored");
  write_comment_line(index_file, comment,
                     "by program. Unless they have colon sign. Then they are "
                     "tags definition.");
  write_comment_line(
      index_file, comment,
      "Ada Web Server template which will be used as HTML template for this");
  write_comment_line(index_file, comment,
                     "file. Required for each file");
  write_comment_line(index_file, comment, "");
  write_comment_line(index_file, comment, "layout: default");
  write_comment_line(index_file, comment, "");
  write_comment_line(
      index_file, comment,
      "You may add as many tags as you want, and they can be in any place in");
  write_comment_line(
      index_file, comment,
      "file, not only at beginning. Tags can be 4 types: strings, boolean,");
  write_comment_line(index_file, comment, "numeric or composite.");
  write_comment_line(
      index_file, comment,
      "First 3 types of tags are in Name: Value scheme. For strings, it can be");
  write_comment_line(index_file, comment,
                     "any alphanumeric value without new line sign. For "
                     "boolean it must be");
  write_comment_line(
      index_file, comment,
      "\"true\" or \"false\", for numeric any number. Program will detect self");
  write_comment_line(index_file, comment,
                     "which type of tag is and properly set it. It always "
                     "falls back to");
  write_comment_line(index_file, comment, "string value.");
  write_comment_line(
      index_file, comment,
      "Composite tags first must be initialized with Name: [] then just add");
  write_comment_line(index_file, comment,
                     "as many as you want values to it by Name: Value scheme.");
  write_comment_line(index_file, comment, "");
  write_comment_line(index_file, comment,
                     "For more information about tags please check program "
                     "documentation.");
  write_comment_line(index_file, comment, "");
  write_comment_line(index_file, comment,
                     "If you have enabled creation of sitemap in the project "
                     "config file,");
  write_comment_line(index_file, comment,
                     "you can set some sitemap parameters too. They are "
                     "defined in this same");
  write_comment_line(index_file, comment,
                     "way like tags, with ParameterName: Value.");
  write_comment_line(index_file, comment, "");
  write_comment_line(
      index_file, comment,
      "priority - The priority of this URL relative to other URLs on your site,");
  write_comment_line(index_file, comment,
                     "           value between 0.0 and 1.0.");
  write_comment_line(
      index_file, comment,
      "changefreq - How frequently the page is likely to change, value can be");
  write_comment_line(
      index_file, comment,
      "             always, hourly, daily, weekly, monthly, yearly or never.");
  write_comment_line(index_file, comment, "");
  write_comment_line(
      index_file, comment,
      "For more information how this options works, please look at the program");
  write_comment_line(index_file, comment, "documentation.");
  write_comment_line(index_file, comment, "");
  write_comment_line(
      index_file, comment,
      "Additionally, you can exclude this file from adding to sitemap by");
  write_comment_line(index_file, comment,
                     "setting option insitemap: false.");
  write_comment_line(index_file, comment, "");
  write_comment_line(
      index_file, comment,
      "If you have enabled creating Atom feed for the site, you must specify");
  write_comment_line(
      index_file, comment,
      "\"title\" tag for this page. If you want to use this file as a main");
  write_comment_line(
      index_file, comment,
      "source of Atom feed, then you must add \"title\" tag for each section");
  write_comment_line(
      index_file, comment,
      "which will be used as source for Atom feed entry. If you want to set");
  write_comment_line(
      index_file, comment,
      "author name for Atom feed, you must add \"author\" tag or setting Author");
  write_comment_line(
      index_file, comment,
      "from configuration file will be used. When you want to set author email");
  write_comment_line(
      index_file, comment,
      "for Atom feed, you must add \"authoremail\" tag. If you want to add");
  write_comment_line(
      index_file, comment,
      "short entry summary, you must add tag \"summary\". Do that tag will be");
  write_comment_line(
      index_file, comment,
      "for whole page or for each entry depends on your Atom feed configuration.");
  write_comment_line(index_file, comment, "");
  write_comment_line(
      index_file, comment,
      "You can also specify canonical link for the page. If you don't set it");
  write_comment_line(
      index_file, comment,
      "here, the program will generate it automatically. To set the default");
  write_comment_line(
      index_file, comment,
      "canonical link for the page set tag \"canonicallink\". It must be a");
  write_comment_line(index_file, comment, "full URL (with https://).");
  write_comment_line(
      index_file, comment,
      "By setting \"author\" tag for the page, you can overwrite the");
  write_comment_line(
      index_file, comment,
      "configuration setting for meta tag author for the page.");
  write_comment_line(index_file, comment, "");
  write_comment_line(index_file, comment, "title: New page");
  write_comment_line(index_file, comment, "");
  write_comment_line(
      index_file, comment,
      "You can without problem delete all this comments from this file.");

  index_file.close();
}

// Helper function to trim whitespace
static std::string trim(const std::string& str) {
  size_t start = 0;
  while (start < str.length() && std::isspace(str[start])) {
    ++start;
  }
  size_t end = str.length();
  while (end > start && std::isspace(str[end - 1])) {
    --end;
  }
  return str.substr(start, end - start);
}

// Helper function to perform simple tag substitution
static std::string substitute_tags(const std::string& content,
                                   const std::map<std::string, std::string>& tags,
                                   const std::string& start_sep,
                                   const std::string& end_sep) {
  std::string result = content;

  // Replace each tag in the content
  for (const auto& [tag_name, tag_value] : tags) {
    std::string tag_placeholder = start_sep + tag_name + end_sep;
    size_t pos = 0;

    while ((pos = result.find(tag_placeholder, pos)) != std::string::npos) {
      result.replace(pos, tag_placeholder.length(), tag_value);
      pos += tag_value.length();
    }
  }

  return result;
}

void create_page(const std::filesystem::path& file_name,
                const std::filesystem::path& directory) {
  try {
    // Calculate output directory and filename
    std::filesystem::path relative_path =
        std::filesystem::relative(file_name.parent_path(), directory);

    std::filesystem::path output_dir =
        std::filesystem::path(config::yass_config.output_directory) / relative_path;

    std::string base_name = file_name.stem().string();
    std::filesystem::path output_file = output_dir / (base_name + ".html");

    // Read the markdown file
    std::ifstream input_file(file_name);
    if (!input_file) {
      messages::show_message("Failed to open file: " + file_name.string(),
                            messages::MessageType::ERROR);
      return;
    }

    // Parse tags from the file
    std::map<std::string, std::string> page_tags;
    std::string content;
    std::string line;
    std::string layout_name = "default";

    const std::string& comment_prefix = config::yass_config.markdown_comment;

    while (std::getline(input_file, line)) {
      // Check if line is a comment (starts with comment prefix)
      if (line.length() >= comment_prefix.length() &&
          line.substr(0, comment_prefix.length()) == comment_prefix) {

        // This is a comment line - check if it has a tag (contains a colon)
        if (line.length() > comment_prefix.length()) {
          // Extract the part after the comment prefix
          std::string after_comment = line.substr(comment_prefix.length());
          // Skip any spaces after the comment prefix
          size_t first_non_space = after_comment.find_first_not_of(' ');
          if (first_non_space != std::string::npos) {
            after_comment = after_comment.substr(first_non_space);
          }

          // Check if this line contains a tag (has a colon)
          size_t colon_pos = after_comment.find(':');
          if (colon_pos != std::string::npos) {
            std::string tag_name = trim(after_comment.substr(0, colon_pos));
            std::string tag_value = trim(after_comment.substr(colon_pos + 1));

            if (!tag_name.empty() && !tag_value.empty()) {
              if (tag_name == "layout") {
                layout_name = tag_value;
              } else {
                page_tags[tag_name] = tag_value;
              }
            }
          }
        }
        // Comment lines (with or without tags) are not included in content
        continue;
      }

      // Non-comment line - include in content
      content += line + "\n";
    }

    input_file.close();

    // Convert markdown to HTML
    std::string html_content =
        cmark::markdown_to_html(content, config::yass_config.html_enabled);

    // Load layout file
    std::filesystem::path layout_path =
        std::filesystem::path(config::yass_config.layouts_directory) /
        (layout_name + ".html");

    if (!std::filesystem::exists(layout_path)) {
      messages::show_message("Layout file not found: " + layout_path.string(),
                            messages::MessageType::ERROR);
      return;
    }

    std::ifstream layout_file(layout_path);
    if (!layout_file) {
      messages::show_message("Failed to open layout: " + layout_path.string(),
                            messages::MessageType::ERROR);
      return;
    }

    std::stringstream layout_buffer;
    layout_buffer << layout_file.rdbuf();
    std::string layout_content = layout_buffer.str();
    layout_file.close();

    // Merge site tags and page tags (page tags override site tags)
    std::map<std::string, std::string> all_tags = config::yass_config.tags;
    for (const auto& [key, value] : page_tags) {
      all_tags[key] = value;
    }

    // Add the content tag
    all_tags["Content"] = html_content;

    // Perform tag substitution
    std::string final_html = substitute_tags(
        layout_content, all_tags,
        config::yass_config.start_tag_separator,
        config::yass_config.end_tag_separator);

    // Create output directory if it doesn't exist
    std::filesystem::create_directories(output_dir);

    // Write output file
    std::ofstream output(output_file);
    if (!output) {
      messages::show_message("Failed to write output: " + output_file.string(),
                            messages::MessageType::ERROR);
      return;
    }

    output << final_html;
    output.close();

  } catch (const std::exception& e) {
    messages::show_message(
        "Error processing page " + file_name.string() + ": " + e.what(),
        messages::MessageType::ERROR);
  }
}

void copy_file(const std::filesystem::path& file_name,
              const std::filesystem::path& directory) {
  try {
    // Calculate output path
    std::filesystem::path relative_path =
        std::filesystem::relative(file_name, directory);

    std::filesystem::path output_path =
        std::filesystem::path(config::yass_config.output_directory) / relative_path;

    // Create output directory if needed
    if (output_path.has_parent_path()) {
      std::filesystem::create_directories(output_path.parent_path());
    }

    // Copy the file
    std::filesystem::copy_file(
        file_name, output_path,
        std::filesystem::copy_options::overwrite_existing);

  } catch (const std::exception& e) {
    messages::show_message(
        "Error copying file " + file_name.string() + ": " + e.what(),
        messages::MessageType::ERROR);
  }
}

}  // namespace pages
