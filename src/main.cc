#include <iostream>
#include <parser.hh>

int main(int argc, char **argv) {
    auto targets = parser();
    std::cout << targets->compiler << std::endl;
    std::cout << targets->standard << std::endl;
    std::cout << targets->targets[0].name << std::endl;
    std::cout << static_cast<int>(targets->targets[0].kind) << std::endl;
    for (auto include_dir : targets->targets[0].include_dirs) {
        std::cout << include_dir.var << std::endl;
        std::cout << static_cast<int>(include_dir.vis) << std::endl;
    }
    for (auto file : targets->targets[0].files) {
        std::cout << file.var << std::endl;
        std::cout << static_cast<int>(file.vis) << std::endl;
    }
    delete targets;
}
