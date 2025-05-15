#ifndef JACL_ROOT_STATEMENT_HPP_
#define JACL_ROOT_STATEMENT_HPP_

#include <variant>

#include "node.hpp"

namespace jacl {

    class NodeRootStatement {
      public:

        JACL_NODE_DEBUG_METHODS(RootStatement)

        bool Parse(class Parser& parser);

        bool Generate(class Generator& generator) const;

        std::variant<Ptr<NodeStatementFuncDecl>, Ptr<NodeStatementFuncDef>> statement;
    };

} // namespace jacl

#endif
