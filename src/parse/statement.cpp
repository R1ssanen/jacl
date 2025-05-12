#include <iostream>

#include "jacldefs.hpp"
#include "parse/node.hpp"
#include "parse/parser.hpp"

namespace jacl {

    bool NodeStatement::Parse(Parser& parser) {
        u64                         mark = parser.Mark();

        NodeStatementFuncDefinition func_def;
        if (func_def.Parse(parser)) {
            m_statement = std::make_unique<NodeStatementFuncDefinition>(std::move(func_def));
            return true;
        }

        NodeStatementReturn ret;
        if (ret.Parse(parser)) {
            m_statement = std::make_unique<NodeStatementReturn>(std::move(ret));
            return true;
        }

        parser.Rewind(mark);
        return false;
    }

    void NodeStatement::DebugPrint(u64 level) const {
        std::string indent(level, ' ');

        std::clog << indent << GetDebugName() << " {\n";
        std::visit([level](auto&& v) { v->DebugPrint(level + 2); }, m_statement);
        std::clog << indent << "}\n";
    }

} // namespace jacl
