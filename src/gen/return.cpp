#include "gen/generator.hpp"
#include "parse/node.hpp"

namespace jacl {

    bool NodeStatementReturn::Generate(Generator& generator) const {
        generator.Emit(Section::TEXT, "; exit statement");
        return true;
    }

} // namespace jacl
