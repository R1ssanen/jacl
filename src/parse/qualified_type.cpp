#include <iostream>

#include "parse/node.hpp"
#include "parse/parser.hpp"

namespace jacl {

    bool NodeQualifiedType::Parse(Parser& parser) {

        Token base_type = parser.Peek();
        if (!base_type.WithinClass(TokenType::BUILTIN)) return false;
        parser.Next();

        m_base_type = base_type;

        if (parser.Peek() == TokenType::DELIMITER_COLON) {
            parser.Next();

            Token qualifier = parser.Peek();
            while (qualifier.WithinClass(TokenType::QUALIFIER)) {
                parser.Next();
                m_qualifiers.push_back(qualifier);
                qualifier = parser.Peek();
            }
        }

        return true;
    }

    void NodeQualifiedType::DebugPrint(u64 level) const {
        std::string indent(level, ' ');

        std::clog << indent << GetDebugName() << ":\n";
        std::clog << indent << "  " << m_base_type.GetDebugName() << ": " << m_base_type.GetLexeme()
                  << '\n';

        std::clog << indent << "  qualifiers:\n";
        for (const auto& qualifier : m_qualifiers)
            std::clog << indent << "    " << qualifier.GetDebugName();
    }

} // namespace jacl
