#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "generator.hpp"
#include "jacldefs.hpp"
#include "lang/program.hpp"
#include "lexer.hpp"
#include "log.hpp"
#include "options.hpp"
#include "parser.hpp"

int main(int argc, char* argv[]) {
    jacl::OptionParser opt(argc, argv);

    const auto&        source_files = opt.GetGroup("--files");
    if (!source_files) {
        JERROR("No source files provided, exiting.");
        return 1;
    }

    const std::string filepath = source_files->front();
    const std::string filename = std::filesystem::path(filepath).stem();

    JLOG("Compiling file '{}'...", filepath);

    std::string jacl_source;
    {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            JERROR("Could not open source file '{}', exiting.", filepath);
            return 1;
        }

        std::ostringstream stream;
        stream << file.rdbuf();
        jacl_source = stream.str();
    }

    jacl::Lexer         lexer(jacl_source);
    jacl::Lexer::Tokens tokens = lexer.Tokenize();

    if (tokens.size() == 0) {
        JERROR("Could not tokenize file '{}', exiting.", filepath);
        return 1;
    }

    if (opt.CheckOption("--flags", "print-tokens")) {
        for (const jacl::Token& token : tokens)
            std::clog << token.GetDebugName() << ": " << token.GetLexeme() << '\n';
        std::clog << '\n';
    }

    jacl::Parser          parser(tokens);
    jacl::Parser::Program program = parser.Parse();

    if (!program) {
        JERROR("Could not parse file '{}', exiting.", filepath);
        return 1;
    }

    if (opt.CheckOption("--flags", "print-ast")) program->DebugPrint();

    jacl::Generator      generator;
    jacl::Generator::ASM asm_source = generator.Generate(program.value());

    if (!asm_source) {
        JERROR("Could not generate file '{}', exiting.", filepath);
        return 1;
    }

    std::string output_base_name;
    if (opt.CheckGroup("--dir")) {
        const std::string                output_dir = std::string(opt.GetOption("--dir").value());
        std::filesystem::directory_entry output_dir_entry(output_dir);

        if (!output_dir_entry.exists()) {
            JERROR("Output directory '{}' does not exist.", output_dir);
            std::string line;

            for (;;) {
                std::clog << "Create? (y/N)\n";
                std::getline(std::cin, line);

                if (line.size() == 0) continue;
                if (std::tolower(line[0]) == 'y') break;
                if (std::tolower(line[0]) == 'n') return 1;
            }

            std::filesystem::create_directory(output_dir);
        }

        else if (!output_dir_entry.is_directory()) {
            JERROR(
                "Invalid output directory specified by '--dir'. '{}' is not a directory.",
                output_dir
            );
            return 1;
        }

        output_base_name = output_dir + "/" + filename;
    } else {
        const std::string parent = std::filesystem::path(filepath).parent_path();
        output_base_name         = parent + "/" + filename;
    }

    const std::string asm_filename = output_base_name + ".s";
    {
        std::ofstream file(asm_filename);
        if (!file.is_open()) {
            JERROR("Could not open or create output file '{}', exiting.", asm_filename);
            return 1;
        }

        file << asm_source.value();
    }

    system(std::format("nasm -felf64 {} -o {}", asm_filename, output_base_name + ".o").c_str());

    const auto& post_builds = opt.GetGroup("--post-build");
    if (post_builds)
        for (const auto& cmd : post_builds.value()) system(cmd.c_str());

    return 0;
}
