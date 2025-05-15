#include "lang/program.hpp"

#include <iostream>
#include <string>

#include "generator.hpp"
#include "jacldefs.hpp"
#include "lang/node.hpp"
#include "lang/root_statement.hpp"
#include "parser.hpp"

namespace jacl {

    bool NodeProgram::Parse(Parser& parser) {

        bool no_errors = true;

        while (parser.NotEnd()) {

            NodeRootStatement root;
            if (root.Parse(parser)) {
                roots.emplace_back(std::make_shared<NodeRootStatement>(root));
            }

            else {
                no_errors = false;
                while ((parser.Peek() != TokenType::DELIMITER_SEMICOLON) &&
                       (parser.Peek() != TokenType::DELIMITER_R_BRACE)) {
                    parser.Next();
                }
                parser.Next();
            }
        }

        return no_errors;
    }

} // namespace jacl

namespace jacl {

    void NodeProgram::DebugPrint(u32 level, u8 indent) const {
        std::clog << std::string(level * indent, ' ') << GetDebugName() << ":\n";

        for (const auto& root : roots) root->DebugPrint(level + 1, indent);
    }

} // namespace jacl

namespace jacl {

    bool NodeProgram::Generate(Generator& generator) const {
        generator.EmitComment(Section::TEXT, "program begin");

        generator.Emit(Section::TEXT, "global", "_start");
        generator.EmitLabel(Section::TEXT, "_start");

        generator.EmitComment(Section::TEXT, "invoke main");
        generator.Emit(Section::TEXT, "call", "main");

        generator.Emit(Section::TEXT, "mov", "rax,", "60");
        generator.Emit(Section::TEXT, "xor", "rdi,", "rdi");
        generator.Emit(Section::TEXT, "syscall");

        for (const auto& root : roots) root->Generate(generator);

        return true;
    }

} // namespace jacl
