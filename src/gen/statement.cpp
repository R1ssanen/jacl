#include "gen/generator.hpp"
#include "parse/node.hpp"

namespace jacl {

    bool NodeStatement::Generate(Generator& generator) const {
        std::visit([&generator](const auto& v) { v->Generate(generator); }, m_statement);
        return true;
    }

} // namespace jacl
