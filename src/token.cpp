#include "token.hpp"

#include <string>

#include "lexer.hpp"

namespace jacl {

    Token::Token(std::string lexeme, TokenType type, u64 begin, u64 end, u64 line, u64 column)
        : m_lexeme(lexeme), m_begin(begin), m_end(end), m_line(line), m_column(column),
          m_type(type) {

        if (m_type == TokenType::IDENTIFIER) {
            m_class      = TokenType::IDENTIFIER;
            m_debug_name = "Token::IDENTIFIER";
        }

        else if (JACL_TYPE_WITHIN(m_type, DELIMITER)) {
            m_class      = TokenType::DELIMITER;
            m_debug_name = "Token::DELIMITER";
        }

        else if (JACL_TYPE_WITHIN(m_type, KEYWORD)) {
            if (JACL_TYPE_WITHIN(m_type, BUILTIN)) {
                m_class      = TokenType::BUILTIN;
                m_debug_name = "Token::BUILTIN_TYPE";
            }

            else {
                m_class      = TokenType::KEYWORD;
                m_debug_name = "Token::KEYWORD";
            }
        }

        else if (JACL_TYPE_WITHIN(m_type, LITERAL)) {
            m_class      = TokenType::LITERAL;
            m_debug_name = "Token::LITERAL";
        }

        else if (JACL_TYPE_WITHIN(m_type, OPERATOR)) {
            if (JACL_TYPE_WITHIN(m_type, ARITHMETIC)) {
                m_class      = TokenType::ARITHMETIC;
                m_debug_name = "Token::OP_ARITHMETIC";
            }

            else if (JACL_TYPE_WITHIN(m_type, LOGICAL)) {
                m_class      = TokenType::LOGICAL;
                m_debug_name = "Token::OP_LOGICAL";
            }

            else {
                m_class      = TokenType::OPERATOR;
                m_debug_name = "Token::OPERATOR";
            }
        }

        else {
            m_class      = TokenType::INVALID;
            m_debug_name = "Token::INVALID";
        }
    }

} // namespace jacl

namespace jacl {

    std::pair<std::string, TokenType> GetSpecialToken(Lexer& lexer) {

        if (lexer.Matches('-', '>')) return { "->", TokenType::OPERATOR_RIGHT_ARROW };
        if (lexer.Matches('<', '-')) return { "<-", TokenType::OPERATOR_RIGHT_ARROW };
        if (lexer.Matches(':', '=')) return { ":=", TokenType::OPERATOR_ASSIGN };

        switch (lexer.Peek()) {
        case '(': return { "(", TokenType::DELIMITER_L_PAREN };
        case ')': return { ")", TokenType::DELIMITER_R_PAREN };
        case '{': return { "{", TokenType::DELIMITER_L_BRACE };
        case '}': return { "}", TokenType::DELIMITER_R_BRACE };
        case ':': return { ":", TokenType::DELIMITER_COLON };
        case ';': return { ";", TokenType::DELIMITER_SEMICOLON };

        case '+': return { "+", TokenType::OPERATOR_PLUS };
        case '-': return { "-", TokenType::OPERATOR_MINUS };
        case '*': return { "*", TokenType::OPERATOR_STAR };
        case '/': return { "/", TokenType::OPERATOR_SLASH };
        case '>': return { ">", TokenType::OPERATOR_GREATER };
        case '<': return { "<", TokenType::OPERATOR_LESSER };

        default: return { nullptr, TokenType::INVALID };
        }
    }

} // namespace jacl
