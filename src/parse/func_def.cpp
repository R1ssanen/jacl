#include <iostream>

#include "parse/node.hpp"
#include "parse/parser.hpp"

namespace jacl {

    bool NodeStatementFuncDefinition::Parse(Parser& parser) {
        u64   mark       = parser.Mark();

        Token identifier = parser.Peek();
        if (identifier != TokenType::IDENTIFIER) return false;
        parser.Next();

        if (!parser.Matches(
                TokenType::DELIMITER_L_PAREN, TokenType::DELIMITER_R_PAREN,
                TokenType::OPERATOR_RIGHT_ARROW
            )) {
            parser.Rewind(mark);
            return false;
        }

        NodeQualifiedType type;
        if (!type.Parse(parser)) {
            parser.Rewind(mark);
            return false;
        }

        NodeScope scope;
        if (!scope.Parse(parser)) {
            parser.Rewind(mark);
            return false;
        }

        m_identifier = identifier;
        m_type       = std::make_unique<NodeQualifiedType>(std::move(type));
        m_scope      = std::make_unique<NodeScope>(std::move(scope));
        return true;
    }

    void NodeStatementFuncDefinition::DebugPrint(u64 level) const {
        std::string indent(level, ' ');

        std::clog << indent << GetDebugName() << " {\n";
        std::clog << indent << "  " << m_identifier.GetDebugName() << ": "
                  << m_identifier.GetLexeme() << '\n';
        m_scope->DebugPrint(level + 2);
        m_type->DebugPrint(level + 2);
        std::clog << indent << "}\n";
    }

} // namespace jacl
