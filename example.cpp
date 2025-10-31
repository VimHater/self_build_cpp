#include <iostream>
#include <vector>
#include "self_rebuild.h"

int main() {
    std::vector<std::string> args = {
        " " + std::string(__FILE__),
        "-o main",
        "-std=c++23",
        "-Wall",
        "-Wextra"
    };
    SELF_REBUILD("g++", args);

    std::cout << "Hello World" << std::endl;
}
