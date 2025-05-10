#ifndef JACL_LEXER_HPP_
#define JACL_LEXER_HPP_

#include <string>
#include <vector>

#include "jacldefs.hpp"
#include "token.hpp"

namespace jacl {

    class Lexer {
      public:

        Lexer(std::string source) : m_source(source), curr(source.front()) { }

        std::vector<Token>& GetTokens() { return m_tokens; }

        bool                Tokenize(void);

        bool                NotEOF(void) const { return m_pos < m_source.size(); }

        char                Peek(u64 ahead = 0) const {
            if (NotEOF()) return m_source[m_pos + ahead];
            return '\0';
        }

        void Next(u64 count = 1) {
            for (u64 i = 0; i < count; ++i) {
                char current = Peek();
                if (!current) return;

                m_pos += 1;
                curr = Peek();

                if (current == '\n') {
                    m_line += 1;
                    m_column = 0;
                }
            }
        }

        void AppendToken(std::string lexeme, TokenType type) {
            m_tokens.emplace_back(lexeme, type, m_line, m_column, m_pos, m_pos + lexeme.size());
        }

      private:

        const std::string  m_source;
        std::vector<Token> m_tokens;
        u64                m_pos    = 0;
        u32                m_line   = 0;
        u32                m_column = 0;
        char               curr;
    };

} // namespace jacl

#endif
