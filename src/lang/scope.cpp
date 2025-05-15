#include "lang/scope.hpp"

#include <iostream>

#include "generator.hpp"
#include "lang/statement.hpp"
#include "parser.hpp"

namespace jacl {

    bool NodeScope::Parse(Parser& parser) {
        u64 mark = parser.Mark();

        if (!parser.Matches(TokenType::DELIMITER_L_BRACE)) {
            parser.Rewind(mark);
            return false;
        }

        NodeStatement statement;
        while (statement.Parse(parser))
            statements.emplace_back(std::make_shared<NodeStatement>(statement));

        if (!parser.Matches(TokenType::DELIMITER_R_BRACE)) {
            parser.Rewind(mark);
            return false;
        }

        return true;
    }

} // namespace jacl

namespace jacl {

    void NodeScope::DebugPrint(u32 level, u8 indent) const {
        std::clog << std::string(level * indent, ' ') << GetDebugName() << ":\n";

        for (const auto& statement : statements) statement->DebugPrint(level + 1, indent);
    }

} // namespace jacl

namespace jacl {

    bool NodeScope::Generate(Generator& generator) const {

        for (const auto& statement : statements) statement->Generate(generator);

        return true;
    }

} // namespace jacl
