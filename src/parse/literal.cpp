#include <cstdlib>

#include "parse/node.hpp"
#include "parse/parser.hpp"

namespace jacl {

    bool NodeLiteral::Parse(Parser& parser) {
        u64   mark    = parser.Mark();

        Token literal = parser.Peek();
        parser.Next();

        switch (literal.GetType()) {

        case TokenType::LITERAL_STRING: m_value = literal.GetLexeme(); return true;

        case TokenType::LITERAL_INT:
            m_value = std::strtoll(literal.GetLexeme().c_str(), nullptr, 10);
            return true;

        case TokenType::LITERAL_FLOAT:
            m_value = std::strtod(literal.GetLexeme().c_str(), nullptr);
            return true;

        default: parser.Rewind(mark); return false;
        }
    }

    void NodeLiteral::DebugPrint(u64 level) const {
        std::string indent(level, ' ');

        std::clog << indent << GetDebugName() << ": ";

        if (std::holds_alternative<i64>(m_value)) std::clog << std::get<i64>(m_value) << '\n';
        else if (std::holds_alternative<f64>(m_value))
            std::clog << std::get<f64>(m_value) << '\n';
        else if (std::holds_alternative<std::string>(m_value))
            std::clog << '"' << std::get<std::string>(m_value) << "\"\n";
        else
            std::clog << '\n';
    }

} // namespace jacl
