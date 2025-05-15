#include "lang/func_decl.hpp"

#include <iostream>

#include "lang/qualified_type.hpp"
#include "parser.hpp"
#include "token.hpp"

namespace jacl {

    bool NodeStatementFuncDecl::Parse(Parser& parser) {
        u64   mark      = parser.Mark();

        Token qualifier = parser.Peek();
        while (qualifier.WithinClass(TokenType::QUALIFIER)) {
            this->qualifiers.push_back(qualifier);
            parser.Next();
            qualifier = parser.Peek();
        }

        this->identifier = parser.Peek();
        if (this->identifier != TokenType::IDENTIFIER) {
            parser.Rewind(mark);
            return false;
        }
        parser.Next();

        if (!parser.Matches(
                TokenType::DELIMITER_L_PAREN, TokenType::DELIMITER_R_PAREN,
                TokenType::OPERATOR_RIGHT_ARROW
            )) {
            parser.Rewind(mark);
            return false;
        }

        NodeQualType qtype;
        if (!qtype.Parse(parser)) {
            parser.Rewind(mark);
            return false;
        }

        this->type = std::make_shared<NodeQualType>(qtype);
        return true;
    }

} // namespace jacl

namespace jacl {

    void NodeStatementFuncDecl::DebugPrint(u32 level, u8 indent) const {
        std::clog << std::string(level * indent, ' ') << GetDebugName() << ": "
                  << identifier.GetLexeme() << '\n';

        type->DebugPrint(level + 1, indent);
    }

} // namespace jacl

namespace jacl {

    bool NodeStatementFuncDecl::Generate(Generator& generator) const { return true; }

} // namespace jacl
