#include "lexer.hpp"

#include <algorithm>
#include <string>

#include "jacldefs.hpp"
#include "token.hpp"

namespace jacl {

    char Lexer::Peek(u64 ahead) const {
        if (NotEOF(ahead)) return m_source[m_index + ahead];
        return '\0';
    }

    void Lexer::Next(u64 count) {
        if (!NotEOF(count)) {
            m_index = m_source.size();
            return;
        }

        for (u64 i = 0; i < count; ++i) {
            char current = m_source[m_index];
            m_index += 1;

            if (current == '\n') {
                m_line += 1;
                m_column = 0;
            }
        }

        curr = m_source[m_index];
    }

} // namespace jacl

namespace jacl {

    std::vector<Token> Lexer::Tokenize(void) {

        std::vector<Token> tokens;

        while (NotEOF()) {

            // comment (single line)
            if (Matches('-', '-')) {
                u64 end = m_source.find_first_of('\n', m_index);
                Next(end - m_index);
            }

            else if (Matches('"')) {
                u64 end    = m_source.find_first_of('"', m_index);
                u64 lenght = end - m_index;
                AppendToken(tokens, m_source.substr(m_index, lenght), TokenType::LITERAL_STRING);
                Next(lenght + 1);
            }

            else if (JACL_SPECIALS.find(curr) != NOT_FOUND) {
                auto [lexeme, type] = GetSpecialToken(*this);
                AppendToken(tokens, lexeme, type);
                Next();
            }

            else if (JACL_DIGITS.find(curr) != NOT_FOUND) {
                u64         end    = m_source.find_first_not_of(JACL_SEPARATED_DIGITS, m_index + 1);
                u64         lenght = end - m_index;

                std::string lexeme = m_source.substr(m_index, lenght);
                lexeme.erase(std::remove(lexeme.begin(), lexeme.end(), '_'), lexeme.end());

                AppendToken(tokens, lexeme, TokenType::LITERAL_INT);
                Next(lenght);
            }

            else if (JACL_ALPHAS.find(curr) != NOT_FOUND) {
                u64         end     = m_source.find_first_not_of(JACL_ALPHANUMERICS, m_index + 1);
                u64         lenght  = end - m_index;
                std::string lexeme  = m_source.substr(m_index, lenght);

                auto        keyword = KEYWORDS.find(lexeme);
                if (keyword == KEYWORDS.end()) AppendToken(tokens, lexeme, TokenType::IDENTIFIER);
                else
                    AppendToken(tokens, lexeme, keyword->second);

                Next(lenght);
            }

            else {
                Next();
            }
        }

        return tokens;
    }

} // namespace jacl
