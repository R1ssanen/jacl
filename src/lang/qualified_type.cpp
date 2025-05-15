#include "lang/qualified_type.hpp"

#include <iostream>

#include "parser.hpp"

namespace jacl {

    bool NodeQualType::Parse(Parser& parser) {
        u64 mark  = parser.Mark();

        base_type = parser.Peek();
        if (!base_type.WithinClass(TokenType::BUILTIN)) {
            parser.Rewind(mark);
            return false;
        }
        parser.Next();

        if (parser.Peek() == TokenType::DELIMITER_COLON) {
            parser.Next();

            Token qualifier = parser.Peek();
            while (qualifier.WithinClass(TokenType::QUALIFIER)) {
                qualifiers.push_back(qualifier);
                parser.Next();
                qualifier = parser.Peek();
            }
        }

        return true;
    }

} // namespace jacl

namespace jacl {

    void NodeQualType::DebugPrint(u32 level, u8 indent) const {
        std::clog << std::string(level * indent, ' ') << GetDebugName() << ": "
                  << base_type.GetLexeme() << '\n';
    }

} // namespace jacl
