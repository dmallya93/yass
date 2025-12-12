#include <gtest/gtest.h>

#include "adachess/version.hpp"

TEST(VersionTest, VersionConstants) {
    EXPECT_EQ(adachess::VERSION, "1.0.0");
    EXPECT_EQ(adachess::NAME, "AdaChess");
    EXPECT_EQ(adachess::DESCRIPTION, "Chess engine");
}

TEST(VersionTest, VersionNotEmpty) {
    EXPECT_FALSE(adachess::VERSION.empty());
    EXPECT_FALSE(adachess::NAME.empty());
    EXPECT_FALSE(adachess::DESCRIPTION.empty());
}
