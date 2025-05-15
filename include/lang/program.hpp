#ifndef JACL_PROGRAM_HPP_
#define JACL_PROGRAM_HPP_

#include <vector>

#include "node.hpp"

namespace jacl {

    class NodeProgram {
      public:

        JACL_NODE_DEBUG_METHODS(Program)

        bool                      Parse(class Parser& parser);

        bool                      Generate(class Generator& generator) const;

        VecPtr<NodeRootStatement> roots;
    };

} // namespace jacl

#endif
