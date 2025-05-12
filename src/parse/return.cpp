#include "parse/node.hpp"
#include "parse/parser.hpp"

namespace jacl {

    bool NodeStatementReturn::Parse(Parser& parser) {
        u64 mark = parser.Mark();

        if (!parser.Matches(TokenType::KEYWORD_RETURN)) return false;

        NodeLiteral value;
        if (!value.Parse(parser)) return false;

        if (!parser.Matches(TokenType::DELIMITER_SEMICOLON)) {
            parser.Rewind(mark);
            return false;
        }

        m_value = std::make_unique<NodeLiteral>(value);
        return true;
    }

    void NodeStatementReturn::DebugPrint(u64 level) const {
        std::string indent(level, ' ');

        std::clog << indent << GetDebugName() << " {\n";
        m_value->DebugPrint(level + 2);
        std::clog << indent << "}\n";
    }

} // namespace jacl
