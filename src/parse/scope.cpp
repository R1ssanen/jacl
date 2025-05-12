#include <iostream>

#include "parse/node.hpp"
#include "parse/parser.hpp"

namespace jacl {

    bool NodeScope::Parse(Parser& parser) {
        u64 mark = parser.Mark();

        if (!parser.Matches(TokenType::DELIMITER_L_BRACE)) {
            parser.Rewind(mark);
            return false;
        }

        NodeStatement statement;
        while (statement.Parse(parser))
            m_statements.push_back(std::make_unique<NodeStatement>(std::move(statement)));

        if (!parser.Matches(TokenType::DELIMITER_R_BRACE)) {
            parser.Rewind(mark);
            return false;
        }

        return true;
    }

    void NodeScope::DebugPrint(u64 level) const {
        std::string indent(level, ' ');

        std::clog << indent << GetDebugName() << " {\n";
        for (const auto& statement : m_statements) statement->DebugPrint(level + 2);
        std::clog << indent << "}\n";
    }

} // namespace jacl
