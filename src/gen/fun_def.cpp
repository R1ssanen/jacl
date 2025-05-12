#include <format>

#include "gen/generator.hpp"
#include "parse/node.hpp"

namespace jacl {

    bool NodeStatementFuncDefinition::Generate(Generator& generator) const {
        generator.Emit(Section::TEXT, "; function definition");
        generator.Emit(Section::TEXT, std::format("{}:", m_identifier.GetLexeme()));

        m_scope->Generate(generator);

        return true;
    }

} // namespace jacl
