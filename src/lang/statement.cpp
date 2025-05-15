#include "lang/statement.hpp"

#include <iostream>

#include "generator.hpp"
#include "jacldefs.hpp"
#include "lang/return.hpp"
#include "lang/scope.hpp"
#include "parser.hpp"

namespace jacl {

    bool NodeStatement::Parse(Parser& parser) {
        u64                 mark = parser.Mark();

        NodeStatementReturn ret;
        if (ret.Parse(parser)) {
            this->statement = std::make_shared<NodeStatementReturn>(ret);
            return true;
        }

        NodeScope scope;
        if (scope.Parse(parser)) {
            this->statement = std::make_shared<NodeScope>(scope);
            return true;
        }

        parser.Rewind(mark);
        return false;
    }

} // namespace jacl

namespace jacl {

    void NodeStatement::DebugPrint(u32 level, u8 indent) const {
        std::clog << std::string(level * indent, ' ') << GetDebugName() << ":\n";

        std::visit([level, indent](const auto& v) { v->DebugPrint(level + 1, indent); }, statement);
    }

} // namespace jacl

namespace jacl {

    bool NodeStatement::Generate(Generator& generator) const {
        std::visit([&generator](const auto& v) { v->Generate(generator); }, statement);
        return true;
    }

} // namespace jacl
