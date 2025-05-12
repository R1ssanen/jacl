#ifndef JACL_LEXER_HPP_
#define JACL_LEXER_HPP_

#include <iostream>
#include <optional>
#include <string>
#include <vector>

#include "jacldefs.hpp"
#include "token.hpp"

namespace jacl {

    class Lexer {
      public:

        using Tokens = std::vector<Token>;

        Lexer(std::string source) : m_source(source), curr(source.front()) { }

        Tokens Tokenize(void);

        bool   NotEOF(u64 ahead = 0) const { return (m_index + ahead) < m_source.size(); }

        char   Peek(u64 ahead = 0) const;

        void   Next(u64 count = 1);

        void   AppendToken(Tokens& tokens, std::string lexeme, TokenType type) {
            tokens.emplace_back(lexeme, type, m_line, m_column, m_index, m_index + lexeme.size());
        }

        template <typename... T> bool Matches(T... Args) {
            char args[] = { Args... };
            u64  count  = sizeof(args) / sizeof(char);

            for (u64 i = 0; i < count; ++i)
                if (Peek(i) == args[i]) continue;
                else
                    return false;

            Next(count);
            return true;
        }

      private:

        std::vector<Token> test;
        const std::string  m_source;
        u64                m_index  = 0;
        u32                m_line   = 0;
        u32                m_column = 0;
        char               curr;
    };

} // namespace jacl

#endif
