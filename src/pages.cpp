// Copyright (c) 2019-2021 Bartek thindil Jasicki <thindil@laeran.pl>
// Copyright (c) 2025 C++ migration
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

#include "yass/pages.hpp"

#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <string>
#include <string_view>

#include "yass/config.hpp"

namespace yass {

void create_empty_file(std::string_view file_name) {
  std::filesystem::path file_path(file_name);

  // If the path doesn't have .md extension, treat it as a directory
  // and create index.md inside it
  if (file_path.extension() != ".md") {
    file_path = file_path / "index.md";
  }

  std::ofstream index_file(file_path);
  if (!index_file) {
    throw std::runtime_error("Failed to create file: " + file_path.string());
  }

  const std::string COMMENT = yass_conf.markdown_comment;

  // Lambda helper to write commented lines
  auto write_comment = [&](std::string_view text) {
    index_file << COMMENT << " " << text << "\n";
  };

  write_comment("All lines which starts with double minus sign are comments and ignored");
  write_comment("by program. Unless they have colon sign. Then they are tags definition.");
  write_comment("Ada Web Server template which will be used as HTML template for this");
  write_comment("file. Required for each file");
  write_comment("");
  write_comment("layout: default");
  write_comment("");
  write_comment("You may add as many tags as you want, and they can be in any place in");
  write_comment("file, not only at beginning. Tags can be 4 types: strings, boolean,");
  write_comment("numeric or composite.");
  write_comment("First 3 types of tags are in Name: Value scheme. For strings, it can be");
  write_comment("any alphanumeric value without new line sign. For boolean it must be");
  write_comment(R"("true" or "false", for numeric any number. Program will detect self)");
  write_comment("which type of tag is and properly set it. It always falls back to");
  write_comment("string value.");
  write_comment("Composite tags first must be initialized with Name: [] then just add");
  write_comment("as many as you want values to it by Name: Value scheme.");
  write_comment("");
  write_comment("For more information about tags please check program documentation.");
  write_comment("");
  write_comment("If you have enabled creation of sitemap in the project config file,");
  write_comment("you can set some sitemap parameters too. They are defined in this same");
  write_comment("way like tags, with ParameterName: Value.");
  write_comment("");
  write_comment("priority - The priority of this URL relative to other URLs on your site,");
  write_comment("           value between 0.0 and 1.0.");
  write_comment("changefreq - How frequently the page is likely to change, value can be");
  write_comment("             always, hourly, daily, weekly, monthly, yearly or never.");
  write_comment("");
  write_comment("For more information how this options works, please look at the program");
  write_comment("documentation.");
  write_comment("");
  write_comment("Additionally, you can exclude this file from adding to sitemap by");
  write_comment("setting option insitemap: false.");
  write_comment("");
  write_comment("If you have enabled creating Atom feed for the site, you must specify");
  write_comment("\"title\" tag for this page. If you want to use this file as a main");
  write_comment("source of Atom feed, then you must add \"title\" tag for each section");
  write_comment("which will be used as source for Atom feed entry. If you want to set");
  write_comment("author name for Atom feed, you must add \"author\" tag or setting Author");
  write_comment("from configuration file will be used. When you want to set author email");
  write_comment("for Atom feed, you must add \"authoremail\" tag. If you want to add");
  write_comment("short entry summary, you must add tag \"summary\". Do that tag will be");
  write_comment("for whole page or for each entry depends on your Atom feed configuration.");
  write_comment("");
  write_comment("You can also specify canonical link for the page. If you don't set it");
  write_comment("here, the program will generate it automatically. To set the default");
  write_comment("canonical link for the page set tag \"canonicallink\". It must be a");
  write_comment("full URL (with https://).");
  write_comment("By setting \"author\" tag for the page, you can overwrite the");
  write_comment("configuration setting for meta tag author for the page.");
  write_comment("");
  write_comment("title: New page");
  write_comment("");
  write_comment("You can without problem delete all this comments from this file.");

  index_file.close();
  if (!index_file) {
    throw std::runtime_error("Failed to write file: " + file_path.string());
  }
}

}  // namespace yass
