#include "gen/generator.hpp"
#include "parse/node.hpp"

namespace jacl {

    bool NodeLiteral::Generate(Generator& generator) const {
        generator.Emit(Section::TEXT, "push", "0");
        return true;
    }

} // namespace jacl
