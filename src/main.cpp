#include <iostream>

#include "adachess/version.hpp"

int main() {
    std::cout << adachess::NAME << " version " << adachess::VERSION << '\n';
    std::cout << adachess::DESCRIPTION << '\n';
    return 0;
}
