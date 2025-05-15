#include "lang/root_statement.hpp"

#include <iostream>
#include <variant>

#include "lang/func_decl.hpp"
#include "lang/func_def.hpp"
#include "lang/scope.hpp"
#include "parser.hpp"

namespace jacl {

    bool NodeRootStatement::Parse(Parser& parser) {
        u64                   mark = parser.Mark();

        NodeStatementFuncDecl declaration;
        if (!declaration.Parse(parser)) return false;

        if (parser.Matches(TokenType::DELIMITER_SEMICOLON)) {
            this->statement = std::make_shared<NodeStatementFuncDecl>(declaration);
            return true;
        }

        NodeScope scope;
        if (!scope.Parse(parser)) {
            parser.Rewind(mark);
            return false;
        }

        NodeStatementFuncDef definition;
        definition.declaration = std::make_shared<NodeStatementFuncDecl>(declaration);
        definition.scope       = std::make_shared<NodeScope>(scope);

        this->statement        = std::make_shared<NodeStatementFuncDef>(definition);
        return true;
    }

} // namespace jacl

namespace jacl {

    void NodeRootStatement::DebugPrint(u32 level, u8 indent) const {
        std::clog << std::string(level * indent, ' ') << GetDebugName() << ":\n";

        std::visit([level, indent](const auto& v) { v->DebugPrint(level + 1, indent); }, statement);
    }

} // namespace jacl

namespace jacl {

    bool NodeRootStatement::Generate(Generator& generator) const {

        std::visit([&generator](const auto& v) { v->Generate(generator); }, statement);

        return true;
    }

} // namespace jacl
