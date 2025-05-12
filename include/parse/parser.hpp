#ifndef JACL_PARSER_HPP_
#define JACL_PARSER_HPP_

#include <iostream>
#include <optional>
#include <vector>

#include "../jacldefs.hpp"
#include "../token.hpp"
#include "node.hpp"

namespace jacl {

    class Parser {
      public:

        using Program = std::optional<NodeProgram>;

        Parser(std::vector<Token>& tokens) : m_tokens(tokens) { }

        template <typename... T> bool Matches(T... Args) {
            TokenType args[] = { Args... };
            u64       count  = sizeof(args) / sizeof(TokenType);

            for (u64 i = 0; i < count; ++i)
                if (Peek(i).GetType() == args[i]) continue;
                else
                    return false;

            Next(count);
            return true;
        }

        bool         NotEnd(u64 ahead = 0) const { return (m_index + ahead) < m_tokens.size(); }

        const Token& Peek(u64 ahead = 0) const;

        void         Next(u64 count = 1);

        u64          Mark(void) const { return m_index; }

        void         Rewind(u64 mark) { m_index = mark; }

        Program      Parse(void);

      private:

        std::vector<Token> m_tokens;
        u64                m_index = 0;
    };

} // namespace jacl

#endif
