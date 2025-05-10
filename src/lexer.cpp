#include "lexer.hpp"

#include <string>

#include "jacldefs.hpp"
#include "token.hpp"

namespace jacl {

    bool Lexer::Tokenize(void) {

        while (NotEOF()) {

            // comment (single line)
            if (curr == '-' && Peek(1) == '-') {
                Next(2);

                u64 end = m_source.find_first_of('\n', m_pos);
                Next(end - m_pos);
            }

            else if (curr == '"') {
                Next();

                u64 end    = m_source.find_first_of('"', m_pos + 1);
                u64 lenght = end - m_pos;
                AppendToken(m_source.substr(m_pos, lenght), TokenType::LITERAL_STRING);
                Next(lenght);
            }

            else if (DELIMITERS.find(curr) != NOT_FOUND || OPERATORS.find(curr) != NOT_FOUND) {
                auto [followed_by, type] = GetCharacterToken(curr);
                AppendToken(m_source.substr(m_pos, 1), type);
                Next();
            }

            else if (DIGITS.find(curr) != NOT_FOUND) {
                u64 end    = m_source.find_first_not_of(DIGITS, m_pos + 1);
                u64 lenght = end - m_pos;
                AppendToken(m_source.substr(m_pos, lenght), TokenType::LITERAL_INT);
                Next(lenght);
            }

            else if (ALPHAS.find(curr) != NOT_FOUND) {
                u64         end     = m_source.find_first_not_of(ALPHANUMERICS, m_pos + 1);
                u64         lenght  = end - m_pos;
                std::string lexeme  = m_source.substr(m_pos, lenght);

                auto        keyword = KEYWORDS.find(lexeme);
                if (keyword == KEYWORDS.end()) AppendToken(lexeme, TokenType::IDENTIFIER);
                else
                    AppendToken(lexeme, keyword->second);

                Next(lenght);
            }

            else {
                Next();
            }
        }

        return true;
    }

} // namespace jacl
