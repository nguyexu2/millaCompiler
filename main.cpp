#include "Parser.hpp"

#include <iostream>

//Use tutorials in: https://llvm.org/docs/tutorial/

int main (int argc, char *argv[]) {
    Parser parser;

    std::cerr << "starting\n";
    if (!parser.Parse()) {
        return 1;
    }
    std::cerr << "done parsing\n";

    parser.Generate().print(outs(), nullptr);
    std::cerr << "done IR\n";

    return 0;
}
