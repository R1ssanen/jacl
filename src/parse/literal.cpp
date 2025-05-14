#include <cstdlib>

#include "parse/node.hpp"
#include "parse/parser.hpp"

namespace jacl {

    bool NodeLiteral::Parse(Parser& parser) {
        u64   mark    = parser.Mark();

        Token literal = parser.Peek();
        if (!literal.WithinClass(TokenType::LITERAL)) return false;
        parser.Next();

        m_string = literal.GetLexeme();

        switch (literal.GetType()) {

        case TokenType::LITERAL_STRING: return true;

        case TokenType::LITERAL_INT:
            m_value = std::strtoll(m_string.c_str(), nullptr, 10);
            return true;

        case TokenType::LITERAL_FLOAT:
            m_value = std::strtod(m_string.c_str(), nullptr);
            return true;

        default: parser.Rewind(mark); return false;
        }
    }

    void NodeLiteral::DebugPrint(u64 level) const {
        std::string indent(level, ' ');

        std::clog << indent << GetDebugName() << ": " << m_string << '\n';
    }

} // namespace jacl
