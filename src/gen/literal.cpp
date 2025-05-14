#include "gen/generator.hpp"
#include "parse/node.hpp"

namespace jacl {

    bool NodeLiteral::Generate(Generator& generator) const {

        std::string value;
        if (std::holds_alternative<i64>(m_value)) value = std::to_string(std::get<i64>(m_value));

        generator.EmitInstruction(Section::TEXT, "push", value.c_str());

        return true;
    }

} // namespace jacl
