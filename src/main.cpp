#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "jacldefs.hpp"
#include "lexer.hpp"

int main(int argc, char* argv[]) {

    if (argc < 2) {
        std::cerr << "error: at least 1 source file required\n";
        return 1;
    }

    const char* filename = argv[1];
    std::string source;
    {
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "error: could not open source file '" << filename << "'\n";
            return 1;
        }

        std::ostringstream stream;
        stream << file.rdbuf();
        source = stream.str();
    }

    jacl::Lexer lexer(source);
    if (!lexer.Tokenize()) {
        std::cerr << "error: could not tokenize file '" << filename << "'\n";
        return 1;
    }

    for (const jacl::Token& token : lexer.GetTokens())
        std::clog << token.GetTypeString() << ": " << token.GetLexeme() << '\n';
}
