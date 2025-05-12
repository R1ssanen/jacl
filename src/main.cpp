#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "gen/generator.hpp"
#include "jacldefs.hpp"
#include "lexer.hpp"
#include "parse/parser.hpp"

int main(int argc, char* argv[]) {

    if (argc < 2) {
        std::cerr << "error: at least 1 source file required\n";
        return 1;
    }

    const char* filename = argv[1];
    std::string jacl_source;
    {
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "error: could not open source file '" << filename << "'\n";
            return 1;
        }

        std::ostringstream stream;
        stream << file.rdbuf();
        jacl_source = stream.str();
    }

    jacl::Lexer         lexer(jacl_source);
    jacl::Lexer::Tokens tokens = lexer.Tokenize();

    if (tokens.size() == 0) {
        std::cerr << "error: could not tokenize file '" << filename << "'\n";
        return 1;
    }

#if 0
    for (const jacl::Token& token : tokens)
        std::clog << token.GetDebugName() << ": " << token.GetLexeme() << '\n';
    std::clog << '\n';
#endif

    jacl::Parser          parser(tokens);
    jacl::Parser::Program program = parser.Parse();

    if (!program.has_value()) {
        std::cerr << "error: could not parse file '" << filename << "'\n";
        return 1;
    }

    program.value().DebugPrint(0);

    jacl::Generator      generator;
    jacl::Generator::ASM asm_source = generator.Generate(program.value());

    if (!asm_source.has_value()) {
        std::cerr << "error: could not generate file '" << filename << "'\n";
        return 1;
    }

    {
        std::ofstream file("out.asm");
        if (!file.is_open()) {
            std::cerr << "error: could not open or create output file '" << "out.asm" << "'\n";
            return 1;
        }

        file << asm_source.value();
    }

    return 0;
}
