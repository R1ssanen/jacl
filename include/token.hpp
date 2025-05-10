#ifndef JACL_TOKEN_HPP_
#define JACL_TOKEN_HPP_

#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>

#include "jacldefs.hpp"

namespace jacl {

    enum class TokenType {
        INVALID = 0,
        IDENTIFIER,

        _BEGIN_DELIMIT, // delimiter

        DELIMIT_L_PAREN,
        DELIMIT_R_PAREN,
        DELIMIT_L_BRACE,
        DELIMIT_R_BRACE,
        DELIMIT_L_BRACKET,
        DELIMIT_R_BRACKET,
        DELIMIT_COLON,
        DELIMIT_SEMICOLON,

        _END_DELIMIT,
        _BEGIN_KEYWORD, // keyword

        KEYWORD_RETURN,
        KEYWORD_GLOBAL,

        _BEGIN_BUILTIN, // keyword - builtin type

        BUILTIN_INT8,
        BUILTIN_INT32,
        BUILTIN_INT64,
        BUILTIN_UINT8,
        BUILTIN_UINT32,
        BUILTIN_UINT64,
        BUILTIN_FLOAT32,
        BUILTIN_FLOAT64,
        BUILTIN_BOOL8,

        _END_BUILTIN,
        _END_KEYWORD,
        _BEGIN_LITERAL, // literals

        LITERAL_INT,
        LITERAL_FLOAT,
        LITERAL_STRING,

        _END_LITERAL,
        _BEGIN_OPERATOR,   // operator
        _BEGIN_ARITHMETIC, // operator - arithmetic

        OPERATOR_PLUS,
        OPERATOR_MINUS,
        OPERATOR_STAR,
        OPERATOR_SLASH,

        _END_ARITHMETIC,
        _BEGIN_LOGICAL, // operator - logical

        OPERATOR_EXCLAMATION,
        OPERATOR_EQUALS,
        OPERATOR_AMPERSAND,
        OPERATOR_BAR,
        OPERATOR_LESSER,
        OPERATOR_GREATER,

        _END_LOGICAL,
        _BEGIN_SPECIAL, // operator - special

        OPERATOR_RETURNS,
        OPERATOR_INDEX,

        _END_SPECIAL,
        _END_OPERATOR,
    };

    inline std::pair<std::optional<std::string>, TokenType> GetCharacterToken(char c) {
        switch (c) {
        case '(': return { {}, TokenType::DELIMIT_L_PAREN };
        case ')': return { {}, TokenType::DELIMIT_R_PAREN };
        case '{': return { {}, TokenType::DELIMIT_L_BRACE };
        case '}': return { {}, TokenType::DELIMIT_R_BRACE };
        case ':': return { {}, TokenType::DELIMIT_COLON };
        case ';': return { {}, TokenType::DELIMIT_SEMICOLON };

        case '+': return { {}, TokenType::OPERATOR_PLUS };
        case '-': return { ">", TokenType::OPERATOR_MINUS };
        case '*': return { {}, TokenType::OPERATOR_STAR };
        case '/': return { {}, TokenType::OPERATOR_SLASH };
        case '>': return { {}, TokenType::OPERATOR_GREATER };
        case '<': return { {}, TokenType::OPERATOR_LESSER };

        default: return { {}, TokenType::INVALID };
        }
    }

    class Token {
      public:

        Token(std::string lexeme, TokenType type, u64 line, u64 column, u64 begin, u64 end)
            : m_lexeme(lexeme), m_type(type), m_begin(begin), m_end(end) { }

        const std::string&    GetLexeme(void) const { return m_lexeme; }

        constexpr std::string GetTypeString(void) const {

            if (m_type == TokenType::INVALID) return "invalid";

            if (m_type == TokenType::IDENTIFIER) return "identifier";

            if (TokenType::_BEGIN_KEYWORD < m_type && m_type < TokenType::_END_KEYWORD)
                return "keyword";

            if (TokenType::_BEGIN_LITERAL < m_type && m_type < TokenType::_END_LITERAL)
                return "literal";

            if (TokenType::_BEGIN_OPERATOR < m_type && m_type < TokenType::_END_OPERATOR)
                return "operator";

            if (TokenType::_BEGIN_DELIMIT < m_type && m_type < TokenType::_END_DELIMIT)
                return "delimiter";

            return "unknown";
        }

      private:

        const std::string m_lexeme;
        u64               m_begin, m_end;
        u64               m_line, m_column;
        TokenType         m_type;
    };

} // namespace jacl

namespace jacl {

    constexpr std::string_view DELIMITERS = ".,[]():;{}";

    constexpr std::string_view OPERATORS  = "+-/*<>";

    constexpr std::string_view DIGITS     = "0123456789";

    constexpr std::string_view ALPHAS     = "_ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

    constexpr std::string_view ALPHANUMERICS =
        "0123456789"
        "_ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

    const std::unordered_map<std::string_view, TokenType> KEYWORDS = {
        { "return", TokenType::KEYWORD_RETURN },
        { "global", TokenType::KEYWORD_GLOBAL },
        { "u32",    TokenType::BUILTIN_UINT32 },
    };

} // namespace jacl

#endif
