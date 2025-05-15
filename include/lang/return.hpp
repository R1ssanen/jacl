#ifndef JACL_RETURN_HPP_
#define JACL_RETURN_HPP_

#include "node.hpp"

namespace jacl {

    class NodeStatementReturn {
      public:

        JACL_NODE_DEBUG_METHODS(Return)

        bool             Parse(class Parser& parser);

        bool             Generate(class Generator& generator) const;

        Ptr<NodeLiteral> literal;
    };

} // namespace jacl

#endif
