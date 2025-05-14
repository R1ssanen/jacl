#include "gen/generator.hpp"
#include "parse/node.hpp"

namespace jacl {

    bool NodeProgram::Generate(Generator& generator) const {
        generator.EmitComment(Section::TEXT, "program begin");

        for (const auto& statement : m_statements) statement->Generate(generator);

        return true;
    }

} // namespace jacl
