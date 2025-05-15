#include "lang/return.hpp"

#include <iostream>

#include "generator.hpp"
#include "jacldefs.hpp"
#include "lang/literal.hpp"
#include "parser.hpp"

namespace jacl {

    bool NodeStatementReturn::Parse(Parser& parser) {
        u64 mark = parser.Mark();

        if (!parser.Matches(TokenType::KEYWORD_RETURN)) {
            parser.Rewind(mark);
            return false;
        }

        NodeLiteral literal;
        if (!literal.Parse(parser)) {
            parser.Rewind(mark);
            return false;
        }

        if (!parser.Matches(TokenType::DELIMITER_SEMICOLON)) {
            parser.Rewind(mark);
            return false;
        }

        this->literal = std::make_shared<NodeLiteral>(literal);
        return true;
    }

} // namespace jacl

namespace jacl {

    void NodeStatementReturn::DebugPrint(u32 level, u8 indent) const {
        std::clog << std::string(level * indent, ' ') << GetDebugName() << ":\n";

        literal->DebugPrint(level + 1, indent);
    }

} // namespace jacl

namespace jacl {

    bool NodeStatementReturn::Generate(Generator& generator) const {
        generator.EmitComment(Section::TEXT, "return statement");

        literal->Generate(generator);

        // epilogue
        generator.Emit(Section::TEXT, "add", "rsp,", "16");
        generator.Emit(Section::TEXT, "leave");
        generator.Emit(Section::TEXT, "ret");

        return true;
    }

} // namespace jacl
