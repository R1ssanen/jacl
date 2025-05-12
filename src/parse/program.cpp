#include <iostream>

#include "jacldefs.hpp"
#include "parse/node.hpp"
#include "parse/parser.hpp"

namespace jacl {

    bool NodeProgram::Parse(Parser& parser) {

        bool no_errors = true;
        while (parser.NotEnd()) {

            NodeStatement statement;
            if (statement.Parse(parser)) {
                m_statements.emplace_back(std::make_unique<NodeStatement>(std::move(statement)));
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

        // return no_errors;
        return true;
    }

    void NodeProgram::DebugPrint(u64 level) const {
        std::string indent(level, ' ');

        std::clog << indent << GetDebugName() << " {\n";
        for (const auto& statement : m_statements) statement->DebugPrint(level + 2);
        std::clog << indent << "}\n";
    }

} // namespace jacl
