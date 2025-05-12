#include "gen/generator.hpp"
#include "parse/node.hpp"

namespace jacl {

    bool NodeScope::Generate(Generator& generator) const {
        generator.Emit(Section::TEXT, "; scope");

        for (const auto& statement : m_statements) statement->Generate(generator);

        return true;
    }

} // namespace jacl
