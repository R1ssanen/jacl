#ifndef JACL_STATEMENT_HPP_
#define JACL_STATEMENT_HPP_

#include <variant>

#include "jacldefs.hpp"
#include "node.hpp"

namespace jacl {

    class NodeStatement {
      public:

        JACL_NODE_DEBUG_METHODS(Statement)

        bool Parse(class Parser& parser);

        bool Generate(class Generator& generator) const;

        std::variant<Ptr<NodeStatementReturn>, Ptr<NodeScope>> statement;
    };

} // namespace jacl

#endif
