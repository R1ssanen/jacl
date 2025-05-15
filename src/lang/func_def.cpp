#include "lang/func_def.hpp"

#include <iostream>

#include "generator.hpp"
#include "lang/func_decl.hpp"
#include "lang/qualified_type.hpp"
#include "lang/scope.hpp"
#include "parser.hpp"

namespace jacl {

    bool NodeStatementFuncDef::Parse(Parser& parser) {
        u64                   mark = parser.Mark();

        NodeStatementFuncDecl decl;
        if (!decl.Parse(parser)) {
            parser.Rewind(mark);
            return false;
        }

        NodeScope scope;
        if (!scope.Parse(parser)) {
            parser.Rewind(mark);
            return false;
        }

        this->declaration = std::make_shared<NodeStatementFuncDecl>(decl);
        this->scope       = std::make_shared<NodeScope>(scope);
        return true;
    }

} // namespace jacl

namespace jacl {

    void NodeStatementFuncDef::DebugPrint(u32 level, u8 indent) const {
        std::clog << std::string(level * indent, ' ') << GetDebugName() << ":\n";

        declaration->DebugPrint(level + 1, indent);
        scope->DebugPrint(level + 1, indent);
    }

} // namespace jacl

namespace jacl {

    bool NodeStatementFuncDef::Generate(Generator& generator) const {
        generator.EmitComment(Section::TEXT, "function definition");

        generator.EmitLabel(Section::TEXT, declaration->identifier.GetLexeme().c_str());

        // prologue
        generator.Emit(Section::TEXT, "push", "rbp");
        generator.Emit(Section::TEXT, "mov", "rbp,", "rsp");
        generator.Emit(Section::TEXT, "sub", "rsp,", "16");

        scope->Generate(generator);

        return true;
    }

} // namespace jacl
