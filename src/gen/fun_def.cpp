#include <format>

#include "gen/generator.hpp"
#include "parse/node.hpp"

namespace jacl {

    bool NodeStatementFuncDefinition::Generate(Generator& generator) const {
        generator.EmitComment(Section::TEXT, "function definition");

        generator.EmitLabel(Section::TEXT, m_identifier.GetLexeme().c_str());
        generator.EmitPrologue();

        m_scope->Generate(generator);

        return true;
    }

} // namespace jacl
