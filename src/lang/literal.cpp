#include "lang/literal.hpp"

#include <cstdlib>
#include <iostream>

#include "generator.hpp"
#include "parser.hpp"

namespace jacl {

    bool NodeLiteral::Parse(Parser& parser) {
        u64   mark    = parser.Mark();

        Token literal = parser.Peek();
        if (!literal.WithinClass(TokenType::LITERAL)) {
            parser.Rewind(mark);
            return false;
        }
        parser.Next();

        string = literal.GetLexeme();

        switch (literal.GetType()) {

        case TokenType::LITERAL_STRING: return true;

        case TokenType::LITERAL_INT: value = std::strtoll(string.c_str(), nullptr, 10); return true;

        case TokenType::LITERAL_FLOAT: value = std::strtod(string.c_str(), nullptr); return true;

        default: parser.Rewind(mark); return false;
        }
    }

} // namespace jacl

namespace jacl {

    void NodeLiteral::DebugPrint(u32 level, u8 indent) const {
        std::clog << std::string(level * indent, ' ') << GetDebugName() << ": " << string << '\n';
    }

} // namespace jacl

namespace jacl {

    bool NodeLiteral::Generate(Generator& generator) const {

        if (!std::holds_alternative<i64>(value)) {
            std::cerr << "gen: cannot generate code for non-integer literals yet.\n";
            return false;
        }

        generator.Emit(Section::TEXT, "push", string.c_str());

        return true;
    }

} // namespace jacl
