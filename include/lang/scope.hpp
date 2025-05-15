#ifndef JACL_SCOPE_HPP_
#define JACL_SCOPE_HPP_

#include "jacldefs.hpp"
#include "node.hpp"

namespace jacl {

    class NodeScope {
      public:

        JACL_NODE_DEBUG_METHODS(Scope)

        bool                  Parse(class Parser& parser);

        bool                  Generate(class Generator& generator) const;

        VecPtr<NodeStatement> statements;
    };

} // namespace jacl

#endif
