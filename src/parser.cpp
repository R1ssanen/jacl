#include "parser.hpp"

#include <optional>

#include "lang/program.hpp"

namespace jacl {

    const Token& Parser::Peek(u64 ahead) const {
        if (NotEnd()) return m_tokens[m_index + ahead];
        return m_tokens.back();
    }

    void Parser::Next(u64 count) {
        if (!NotEnd(count)) m_index = m_tokens.size();
        else
            m_index += count;
    }

    Parser::Program Parser::Parse(void) {

        NodeProgram program;
        if (program.Parse(*this)) return program;
        else
            return std::nullopt;
    }

} // namespace jacl
