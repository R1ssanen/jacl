#ifndef JACL_TOKEN_HPP_
#define JACL_TOKEN_HPP_

#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>

#include "jacldefs.hpp"

#define JACL_TYPE_WITHIN(type, class) ((type > TokenType::class) && (type < TokenType::END_##class))

namespace jacl {

    enum class TokenType {
        INVALID = 0,
        IDENTIFIER, // identifier

        DELIMITER, // delimiter
        DELIMITER_L_PAREN,
        DELIMITER_R_PAREN,
        DELIMITER_L_BRACE,
        DELIMITER_R_BRACE,
        DELIMITER_L_BRACKET,
        DELIMITER_R_BRACKET,
        DELIMITER_COLON,
        DELIMITER_SEMICOLON,
        END_DELIMITER,

        KEYWORD, // keyword
        KEYWORD_RETURN,

        QUALIFIER, // qualifier
        QUALIFIER_GLOBAL,
        QUALIFIER_LOCAL,
        QUALIFIER_EXTERN,
        END_QUALIFIER,

        BUILTIN, // keyword - builtin type
        BUILTIN_INT8,
        BUILTIN_INT32,
        BUILTIN_INT64,
        BUILTIN_UINT8,
        BUILTIN_UINT32,
        BUILTIN_UINT64,
        BUILTIN_FLOAT32,
        BUILTIN_FLOAT64,
        BUILTIN_BOOL8,
        END_BUILTIN,

        END_KEYWORD,

        LITERAL, // literals
        LITERAL_INT,
        LITERAL_FLOAT,
        LITERAL_STRING,
        END_LITERAL,

        OPERATOR, // operator
        OPERATOR_INDEX,
        OPERATOR_RIGHT_ARROW,
        OPERATOR_LEFT_ARROW,
        OPERATOR_ASSIGN,

        ARITHMETIC, // operator - arithmetic
        OPERATOR_PLUS,
        OPERATOR_MINUS,
        OPERATOR_STAR,
        OPERATOR_SLASH,
        END_ARITHMETIC,

        LOGICAL, // operator - logical
        OPERATOR_EXCLAMATION,
        OPERATOR_EQUALS,
        OPERATOR_AMPERSAND,
        OPERATOR_BAR,
        OPERATOR_LESSER,
        OPERATOR_GREATER,
        END_LOGICAL,

        END_OPERATOR,
    };

    class Token {
      public:

        Token(std::string lexeme, TokenType type, u64 begin, u64 end, u64 line, u64 column);

        Token() = default;

        const std::string&  GetLexeme(void) const { return m_lexeme; }

        const std::string&  GetDebugName(void) const { return m_debug_name; }

        std::pair<u64, u64> GetPosition(void) const { return { m_line, m_column }; }

        TokenType           GetType(void) const { return m_type; }

        bool                WithinClass(TokenType _class) const { return m_class == _class; }

        bool                operator==(TokenType type) const { return m_type == type; }

        bool                operator!=(TokenType type) const { return m_type != type; }

      private:

        std::string m_lexeme;
        std::string m_debug_name;
        u64         m_begin, m_end;
        u64         m_line, m_column;
        TokenType   m_type, m_class;
    };

    std::pair<std::string, TokenType> GetSpecialToken(class Lexer& lexer);

} // namespace jacl

namespace jacl {

    using namespace std::literals;

#define JACL_SPECIALS                                                                              \
    ".,:;[](){}"sv                                                                                 \
    "+-/*<>"sv

#define JACL_DIGITS "0123456789"sv

#define JACL_SEPARATED_DIGITS                                                                      \
    JACL_DIGITS                                                                                    \
    "_"sv

#define JACL_ALPHAS "_ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"sv

#define JACL_ALPHANUMERICS                                                                         \
    JACL_DIGITS                                                                                    \
    JACL_ALPHAS

    const std::unordered_map<std::string, TokenType> KEYWORDS = {
        { "return", TokenType::KEYWORD_RETURN   },

        { "u32",    TokenType::BUILTIN_UINT32   },

        { "global", TokenType::QUALIFIER_GLOBAL },
        { "local",  TokenType::QUALIFIER_LOCAL  },
        { "extern", TokenType::QUALIFIER_EXTERN },
    };

} // namespace jacl

#endif
