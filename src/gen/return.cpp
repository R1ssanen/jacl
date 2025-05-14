#include "gen/generator.hpp"
#include "parse/node.hpp"

namespace jacl {

    bool NodeStatementReturn::Generate(Generator& generator) const {
        generator.EmitComment(Section::TEXT, "return statement");

        m_value->Generate(generator);
        generator.EmitEpilogue();

        return true;
    }

} // namespace jacl
