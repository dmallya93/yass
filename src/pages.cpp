// Copyright (c) 2019-2021 Bartek thindil Jasicki <thindil@laeran.pl>
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
#include "yass/config.hpp"
#include <filesystem>
#include <fstream>
#include <stdexcept>

namespace yass {

void create_empty_file(const std::string& file_name) {
    namespace fs = std::filesystem;

    fs::path file_path(file_name);

    // If the file_name is a directory or has no extension, create index.md inside it
    // Otherwise, use the file path as-is (assuming it already has .md extension)
    fs::path target_path;
    if (fs::is_directory(file_path) || file_path.extension().empty()) {
        target_path = file_path / "index.md";
    } else {
        target_path = file_path;
    }

    std::ofstream index_file(target_path);
    if (!index_file) {
        throw std::runtime_error("Failed to create file: " + target_path.string());
    }

    const std::string COMMENT = yass_conf.markdown_comment;

    auto pl = [&](const std::string& item) {
        index_file << COMMENT << " " << item << "\n";
    };

    pl("All lines which starts with double minus sign are comments and ignored");
    pl("by program. Unless they have colon sign. Then they are tags definition.");
    pl("Ada Web Server template which will be used as HTML template for this");
    pl("file. Required for each file");
    pl("");
    pl("layout: default");
    pl("");
    pl("You may add as many tags as you want, and they can be in any place in");
    pl("file, not only at beginning. Tags can be 4 types: strings, boolean,");
    pl("numeric or composite.");
    pl("First 3 types of tags are in Name: Value scheme. For strings, it can be");
    pl("any alphanumeric value without new line sign. For boolean it must be");
    pl("\"true\" or \"false\", for numeric any number. Program will detect self");
    pl("which type of tag is and properly set it. It always falls back to");
    pl("string value.");
    pl("Composite tags first must be initialized with Name: [] then just add");
    pl("as many as you want values to it by Name: Value scheme.");
    pl("");
    pl("For more information about tags please check program documentation.");
    pl("");
    pl("If you have enabled creation of sitemap in the project config file,");
    pl("you can set some sitemap parameters too. They are defined in this same");
    pl("way like tags, with ParameterName: Value.");
    pl("");
    pl("priority - The priority of this URL relative to other URLs on your site,");
    pl("           value between 0.0 and 1.0.");
    pl("changefreq - How frequently the page is likely to change, value can be");
    pl("             always, hourly, daily, weekly, monthly, yearly or never.");
    pl("");
    pl("For more information how this options works, please look at the program");
    pl("documentation.");
    pl("");
    pl("Additionally, you can exclude this file from adding to sitemap by");
    pl("setting option insitemap: false.");
    pl("");
    pl("If you have enabled creating Atom feed for the site, you must specify");
    pl("\"title\" tag for this page. If you want to use this file as a main");
    pl("source of Atom feed, then you must add \"title\" tag for each section");
    pl("which will be used as source for Atom feed entry. If you want to set");
    pl("author name for Atom feed, you must add \"author\" tag or setting Author");
    pl("from configuration file will be used. When you want to set author email");
    pl("for Atom feed, you must add \"authoremail\" tag. If you want to add");
    pl("short entry summary, you must add tag \"summary\". Do that tag will be");
    pl("for whole page or for each entry depends on your Atom feed configuration.");
    pl("");
    pl("You can also specify canonical link for the page. If you don't set it");
    pl("here, the program will generate it automatically. To set the default");
    pl("canonical link for the page set tag \"canonicallink\". It must be a");
    pl("full URL (with https://).");
    pl("By setting \"author\" tag for the page, you can overwrite the");
    pl("configuration setting for meta tag author for the page.");
    pl("");
    pl("title: New page");
    pl("");
    pl("You can without problem delete all this comments from this file.");
}

}  // namespace yass
