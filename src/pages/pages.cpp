#include "pages.hpp"

#include <fstream>
#include <stdexcept>

#include "config.hpp"

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

}  // namespace pages
